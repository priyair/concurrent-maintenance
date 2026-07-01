#include "manager.hpp"

#include <phosphor-logging/lg2.hpp>

#include <string_view>

namespace concurrent_maintenance
{

constexpr auto readyToRemoveProperty = "ReadyToRemove";

Manager::Manager(sdbusplus::async::context& ctx) :
    ctx(ctx), cmObjectManager(ctx)
{
    lg2::info("Concurrent Maintenance manager initialized");

    /* Create property change signal match for ReadyToRemove property
     * This will match all child objects under /xyz/openbmc_project/inventory
     * that implement xyz.openbmc_project.State.ReadyToRemove interface
     */
    readyToRemoveMatch = std::make_unique<sdbusplus::async::match>(
        ctx,
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

        const auto it = changedProperties.find(readyToRemoveProperty);
        if (it != changedProperties.end())
        {
            bool readyToRemove = std::get<bool>(it->second);
            const auto& objectPath = msg.get_path();
            lg2::info("ReadyToRemove property changed on {PATH}: {VALUE}",
                      "PATH", objectPath, "VALUE", readyToRemove);

            // Manage CM object based on property value
            manageCMObject(readyToRemove);
        }
    }
    catch (const std::exception& e)
    {
        lg2::error("Error handling ReadyToRemove property change: {ERROR}",
                   "ERROR", e);
    }
}

void Manager::manageCMObject(bool readyToRemove)
{
    try
    {
        // Check if a CM object already exists
        if (currentCMObject)
        {
            lg2::error(
                "CM is already in progress. Object already exists at path: {PATH}.",
                "PATH", currentCMObject->getPath());
            return;
        }

        // Call CMObjectManager to create CM object
        currentCMObject = cmObjectManager.createCMObject(readyToRemove);

        lg2::info("CM object created at {PATH}", "PATH",
                  currentCMObject->getPath());
    }
    catch (const std::exception& e)
    {
        lg2::error("Error managing CM object: {ERROR}", "ERROR", e);
    }
}

} // namespace concurrent_maintenance
