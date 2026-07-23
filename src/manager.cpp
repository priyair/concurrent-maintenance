#include "manager.hpp"

#include <phosphor-logging/lg2.hpp>
#include <sdbusplus/async.hpp>
#include <sdbusplus/async/timer.hpp>
#include <sdbusplus/bus.hpp>
#include <sdbusplus/message.hpp>

#include <chrono>
#include <string_view>

using namespace std::chrono_literals;

namespace concurrent_maintenance
{

constexpr auto readyToRemoveProperty = "ReadyToRemove";

Manager::Manager(sdbusplus::async::context& ctx) :
    ctx(ctx), currentCMHandler(nullptr)
{
    lg2::info("Concurrent Maintenance manager initialized");

    auto& bus = ctx.get_bus();

    readyToRemoveMatch = std::make_unique<sdbusplus::bus::match_t>(
        bus,
        sdbusplus::bus::match::rules::propertiesChangedNamespace(
            "/xyz/openbmc_project/inventory",
            "xyz.openbmc_project.State.ReadyToRemove"),
        [this](sdbusplus::message_t& msg) { handleReadyToRemoveChange(msg); });

    lg2::info(
        "ReadyToRemove property watcher registered for all inventory objects");
}

void Manager::handleReadyToRemoveChange(sdbusplus::message_t& msg)
{
    std::string interface;
    std::map<std::string, std::variant<bool>> changedProperties;

    try
    {
        msg.read(interface, changedProperties);

        auto it = changedProperties.find(readyToRemoveProperty);
        if (it != changedProperties.end())
        {
            bool readyToRemove = std::get<bool>(it->second);
            std::string fruPath = msg.get_path();

            lg2::info("ReadyToRemove property changed on {PATH}: {VALUE}",
                      "PATH", fruPath, "VALUE", readyToRemove);

            ctx.spawn(handleAsync(readyToRemove, std::move(fruPath)));
        }
    }
    catch (const std::exception& e)
    {
        lg2::error("Error handling ReadyToRemove property change: {ERROR}",
                   "ERROR", e);
    }
}

sdbusplus::async::task<> Manager::handleAsync(bool readyToRemove,
                                              std::string fruPath)
{
    /* Yield for 0ms so that back-to-back signals (concurrent requests)
     * both get their handleAsync coroutines spawned and suspended here
     * before either one resumes into manageCMHandler. This guarantees
     * the first coroutine sets currentCMHandler and the second sees it
     * and is rejected by the concurrency gate.
     * NOTE: In production, real FSI app D-Bus calls inside execute()
     * provide natural suspension points. This sleep covers the stub path.
     */
    co_await sdbusplus::async::sleep_for(ctx, 0ms);
    manageCMHandler(readyToRemove, fruPath);
}

void Manager::manageCMHandler(bool readyToRemove, const std::string& fruPath)
{
    if (currentCMHandler)
    {
        lg2::error(
            "CM is already in progress. Dropping new request for {FRUPATH}.",
            "FRUPATH", fruPath);
        return;
    }

    currentCMHandler = CMHandler::create(ctx, fruPath);

    if (!currentCMHandler)
    {
        return;
    }

    lg2::info("CM handler created for {PATH}", "PATH", fruPath);

    ctx.spawn(runHandler(readyToRemove));
}

sdbusplus::async::task<> Manager::runHandler(bool isRemove)
{
    // Cache the path before co_await so the log is safe even if
    // currentCMHandler is reset in a future refactor.
    const std::string path = currentCMHandler->getFruPath();

    try
    {
        co_await currentCMHandler->execute(isRemove);

        lg2::info("CM operation completed for {PATH}", "PATH", path);
    }
    catch (const std::exception& e)
    {
        lg2::error("CM operation failed for {PATH}: {ERROR}", "PATH", path,
                   "ERROR", e.what());
    }

    currentCMHandler = nullptr;

    co_return;
}

} // namespace concurrent_maintenance
