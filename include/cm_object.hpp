#pragma once

#include <sdbusplus/async/context.hpp>
#include <sdbusplus/async/task.hpp>
#include <sdbusplus/async/timer.hpp>
#include <sdbusplus/bus.hpp>

#include <string>

namespace concurrent_maintenance
{

/**
 * @brief Tracks one in-flight CM operation (remove or add).
 *
 * CMObject owns:
 *   - The D-Bus object path (/com/ibm/concurrent_maintenance/remove or /add)
 *   - Progress state for the operation (updated by the concrete handler
 *     via setStep() as each FSI/VPD step completes).
 *
 * CMHandler::createCMObject() constructs this object.
 * CMHandler::execute() owns the lifetime of this object for the duration
 * of the operation.
 *
 * NOTE: Progress D-Bus interface will be added in a follow-on PR (see TODO).
 */
class CMObject
{
  public:
    CMObject(sdbusplus::async::context& ctx, const std::string& path);

    CMObject(const CMObject&) = delete;
    CMObject& operator=(const CMObject&) = delete;
    CMObject(CMObject&&) = delete;
    CMObject& operator=(CMObject&&) = delete;

    ~CMObject() = default;

    /** @brief D-Bus object path for this operation. */
    const std::string& getPath() const
    {
        return objectPath;
    }

    // TODO: Add progress interface when implementing progress tracking.
    // Handlers will call setStep(n) / setCompleted() / setFailed() here.
    // When adding the progress interface, use sdbusplus::server::object_t:
    // Example:
    // sdbusplus::server::object_t<xyz::openbmc_project::Common::Progress>
    //     dbusObject(bus, path.c_str(), action::defer_emit);
    // dbusObject.emit_object_added();

  private:
    sdbusplus::async::context& ctx;
    std::string objectPath;
};

} // namespace concurrent_maintenance
