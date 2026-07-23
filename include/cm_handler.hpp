#pragma once

#include "cm_object.hpp"
#include "fru_identifier.hpp"

#include <sdbusplus/async/context.hpp>
#include <sdbusplus/async/task.hpp>

#include <memory>
#include <string>

namespace concurrent_maintenance
{

/**
 * @brief Abstract base for per-FRU-type CM operation handlers.
 *
 * CMHandler owns the full factory + execution pipeline:
 *
 *   1. FRU type determination
 *      CMHandler::create() calls FRUIdentifier::identifyType() and
 *      instantiates the matching concrete subclass.
 *      FRUIdentifier remains a standalone utility with its own tests.
 *
 *   2. CMObject creation
 *      CMHandler::createCMObject() builds the D-Bus tracking object
 *      (/com/ibm/concurrent_maintenance/remove or /add).
 *
 *   3. Per-type operation sequence
 *      Concrete subclasses implement:
 *        performRemove(CMObject&) -> sdbusplus::async::task<>
 *        performAdd(CMObject&)    -> sdbusplus::async::task<>
 *
 *      Returning task<> (not void) lets the caller co_await each step,
 *      which is required once the bodies make real FSI app D-Bus calls
 *      (themselves co_await operations). CMObject& is passed by reference
 *      rather than stored at construction so handlers remain stateless
 *      with respect to the operation — easier to unit-test.
 *
 * Concrete subclasses:
 *   - FSICardCMHandler     - kills/enables FSI links, deletes/collects VPD
 *   - BMCCMHandler         - same sequence for the BMC card FRU
 *   - SwitchboardCMHandler - same sequence for the switchboard FRU
 *
 * Manager calls CMHandler::create(), gate-keeps concurrency, then
 * calls execute() which drives the full sequence.
 */
class CMHandler
{
  public:
    CMHandler(const CMHandler&) = delete;
    CMHandler& operator=(const CMHandler&) = delete;
    CMHandler(CMHandler&&) = delete;
    CMHandler& operator=(CMHandler&&) = delete;

    virtual ~CMHandler() = default;

    /**
     * @brief Factory: identify FRU type and return the right handler.
     *
     * Calls FRUIdentifier::identifyType(fruPath) and constructs the
     * matching concrete subclass.
     *
     * @param ctx      sdbusplus async context (forwarded to CMObject).
     * @param fruPath  Inventory D-Bus path of the affected FRU.
     * @return Concrete CMHandler subclass, or nullptr for UNKNOWN type.
     */
    static std::unique_ptr<CMHandler> create(sdbusplus::async::context& ctx,
                                             const std::string& fruPath);

    /**
     * @brief Create the D-Bus tracking object for this operation.
     *
     * @param isRemove  true  -> /com/ibm/concurrent_maintenance/remove
     *                  false -> /com/ibm/concurrent_maintenance/add
     * @return Owning pointer to the new CMObject.
     */
    std::unique_ptr<CMObject> createCMObject(bool isRemove) const;

    /**
     * @brief Drive the full remove or add sequence asynchronously.
     *
     * Creates a CMObject, yields to the event loop (so concurrent
     * signals are dispatched and rejected before work begins), then
     * co_awaits performRemove(cmObj) or performAdd(cmObj).
     *
     * @param isRemove  true -> removal; false -> addition.
     */
    sdbusplus::async::task<> execute(bool isRemove);

    /** @brief Inventory D-Bus path of the FRU under maintenance. */
    const std::string& getFruPath() const
    {
        return fruPath;
    }

  protected:
    explicit CMHandler(sdbusplus::async::context& ctx,
                       const std::string& fruPath) : ctx(ctx), fruPath(fruPath)
    {}

    /**
     * @brief Execute the ordered removal steps for this FRU type.
     *
     * Returns task<> so the caller (execute()) can co_await it.
     * This is required for real FSI app D-Bus calls which are
     * themselves async operations.
     *
     * Typical sequence:
     *   1. co_await fsiAppProxy.killFSILinks(fruPath)
     *   2. co_await fsiAppProxy.deleteVPD(fruPath)
     *   3. Presence-detect (FSI app) sets Available=false on the
     *      inventory object.
     *
     * @param cmObj  In-flight CMObject; use to update progress at
     *               each step once the Progress interface is added.
     */
    virtual sdbusplus::async::task<> performRemove(CMObject& cmObj) = 0;

    /**
     * @brief Execute the ordered addition steps for this FRU type.
     *
     * Returns task<> so the caller (execute()) can co_await it.
     *
     * Typical sequence:
     *   1. co_await fsiAppProxy.enableFSILinks(fruPath)
     *   2. co_await fsiAppProxy.collectVPD(fruPath)
     *   3. Presence-detect (FSI app) sets Available=true on the
     *      inventory object.
     *
     * @param cmObj  In-flight CMObject; use to update progress at
     *               each step once the Progress interface is added.
     */
    virtual sdbusplus::async::task<> performAdd(CMObject& cmObj) = 0;

    sdbusplus::async::context& ctx;
    std::string fruPath;
};

} // namespace concurrent_maintenance
