#include "cm_object.hpp"

#include <phosphor-logging/lg2.hpp>

namespace concurrent_maintenance
{

CMObject::CMObject(sdbusplus::async::context& ctx, const std::string& path) :
    ctx(ctx), objectPath(path)
{
    lg2::info("CM object created at path: {PATH}", "PATH", path);

    // TODO: When implementing progress interface, create the D-Bus object
    // here. Example:
    //   auto& bus = ctx.get_bus();
    //   dbusObject = std::make_unique<sdbusplus::server::object_t<...>>(
    //       bus, path.c_str(),
    //       sdbusplus::server::object_t<...>::action::defer_emit);
    //   dbusObject->emit_object_added();
}

} // namespace concurrent_maintenance
