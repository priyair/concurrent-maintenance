#include "cm_handler.hpp"

#include "bmc_cm_handler.hpp"
#include "fsi_card_cm_handler.hpp"
#include "switchboard_cm_handler.hpp"

#include <phosphor-logging/lg2.hpp>
#include <sdbusplus/async/timer.hpp>

#include <chrono>

using namespace std::chrono_literals;

namespace concurrent_maintenance
{

constexpr auto cmRemoveObjectPath = "/com/ibm/concurrent_maintenance/remove";
constexpr auto cmAddObjectPath = "/com/ibm/concurrent_maintenance/add";

// static factory
std::unique_ptr<CMHandler> CMHandler::create(sdbusplus::async::context& ctx,
                                             const std::string& fruPath)
{
    FRUType fruType = FRUIdentifier::identifyType(fruPath);

    switch (fruType)
    {
        case FRUType::FSI:
            lg2::info("CMHandler: creating FSICardCMHandler for {PATH}", "PATH",
                      fruPath);
            return std::make_unique<FSICardCMHandler>(ctx, fruPath);

        case FRUType::BMC:
            lg2::info("CMHandler: creating BMCCMHandler for {PATH}", "PATH",
                      fruPath);
            return std::make_unique<BMCCMHandler>(ctx, fruPath);

        case FRUType::SWITCHBOARD:
            lg2::info("CMHandler: creating SwitchboardCMHandler for {PATH}",
                      "PATH", fruPath);
            return std::make_unique<SwitchboardCMHandler>(ctx, fruPath);

        case FRUType::UNKNOWN:
        default:
            lg2::error(
                "CMHandler: unrecognized FRU type for {PATH} - no handler",
                "PATH", fruPath);
            return nullptr;
    }
}

// createCMObject
std::unique_ptr<CMObject> CMHandler::createCMObject(bool isRemove) const
{
    const std::string path = isRemove ? cmRemoveObjectPath : cmAddObjectPath;
    lg2::info("CMHandler: creating CM object at {PATH}", "PATH", path);
    return std::make_unique<CMObject>(ctx, path);
}

// execute
sdbusplus::async::task<> CMHandler::execute(bool isRemove)
{
    auto cmObj = createCMObject(isRemove);

    /* No yield here — the concurrency gate yield is already in
     * Manager::handleAsync() (sleep_for 0ms). A second yield here
     * would be a redundant event-loop round-trip on every operation.
     * Real FSI app D-Bus calls inside performRemove/performAdd will
     * provide natural co_await suspension points.
     */

    try
    {
        if (isRemove)
        {
            lg2::info("CMHandler: starting remove for {PATH}", "PATH", fruPath);
            co_await performRemove(*cmObj);
        }
        else
        {
            lg2::info("CMHandler: starting add for {PATH}", "PATH", fruPath);
            co_await performAdd(*cmObj);
        }

        lg2::info("CMHandler: sequence completed for {PATH}", "PATH", fruPath);
        // TODO: cmObj->setCompleted() when Progress PR lands.
    }
    catch (const std::exception& e)
    {
        lg2::error("CMHandler: sequence failed for {PATH}: {ERROR}", "PATH",
                   fruPath, "ERROR", e.what());
        // TODO: cmObj->setFailed() when Progress PR lands.
        // At this point the Progress object remains in InProgress state
        // until the Progress PR adds the setFailed() call here.
    }

    co_return;
}

} // namespace concurrent_maintenance
