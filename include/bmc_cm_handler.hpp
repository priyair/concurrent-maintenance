#pragma once

#include "cm_handler.hpp"

namespace concurrent_maintenance
{

/**
 * @brief CM handler for the BMC card FRU.
 *
 * performRemove and performAdd return sdbusplus::async::task<> so
 * the real FSI app D-Bus calls can be co_awaited inside them in a
 * follow-on PR.
 */
class BMCCMHandler : public CMHandler
{
  public:
    BMCCMHandler(sdbusplus::async::context& ctx, const std::string& fruPath) :
        CMHandler(ctx, fruPath)
    {}

    sdbusplus::async::task<> performRemove(CMObject& cmObj) override;
    sdbusplus::async::task<> performAdd(CMObject& cmObj) override;
};

} // namespace concurrent_maintenance
