#include "manager.hpp"

#include <phosphor-logging/lg2.hpp>
#include <sdbusplus/async/context.hpp>
#include <sdbusplus/server/manager.hpp>

int main()
{
    sdbusplus::async::context ctx;

    // Create ObjectManager for concurrent maintenance dbus objects
    constexpr auto objManagerPath = "/com/ibm/ConcurrentMaintenance";
    sdbusplus::server::manager_t dbusObjManager(ctx, objManagerPath);

    concurrent_maintenance::Manager cmManager(ctx);

    ctx.request_name("com.ibm.ConcurrentMaintenance");

    lg2::info("Concurrent Maintenance service started");

    ctx.run();

    return 0;
}
