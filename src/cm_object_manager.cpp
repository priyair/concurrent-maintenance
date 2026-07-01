#include "cm_object_manager.hpp"

#include <phosphor-logging/lg2.hpp>

namespace concurrent_maintenance
{

constexpr auto cmRemoveObjectPath = "/com/ibm/ConcurrentMaintenance/remove";
constexpr auto cmAddObjectPath = "/com/ibm/ConcurrentMaintenance/add";

CMObjectManager::CMObjectManager(sdbusplus::async::context& ctx) : ctx(ctx)
{
    lg2::info("CMObjectManager initialized");
}

std::unique_ptr<CMObject> CMObjectManager::createCMObject(bool isRemove)
{
    const std::string cmObjectPath = isRemove ? cmRemoveObjectPath
                                              : cmAddObjectPath;

    lg2::info("CM object path: {PATH}", "PATH", cmObjectPath);

    return std::make_unique<CMObject>(ctx, cmObjectPath);
}

} // namespace concurrent_maintenance
