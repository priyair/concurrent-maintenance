#include "cm_object.hpp"

#include <phosphor-logging/lg2.hpp>

#include <chrono>

namespace concurrent_maintenance
{

CMObject::CMObject(sdbusplus::async::context& /*ctx*/,
                   const std::string& path) : objectPath(path)
{
    lg2::info("Creating object at path: {PATH}", "PATH", path);

    // Create CM object

    lg2::info("Object created at path: {PATH}", "PATH", path);
}

} // namespace concurrent_maintenance
