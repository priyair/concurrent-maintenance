#pragma once

#include "cm_handler.hpp"

namespace concurrent_maintenance
{

/**
 * @brief CM handler for FSI card FRUs.
 *
 * performRemove and performAdd return sdbusplus::async::task<> so
 * the real FSI app D-Bus calls (kill/enable FSI links, delete/collect
 * VPD) can be co_awaited inside them in a follow-on PR.
 */
class FSICardCMHandler : public CMHandler
{
  public:
    FSICardCMHandler(sdbusplus::async::context& ctx,
                     const std::string& fruPath) : CMHandler(ctx, fruPath)
    {}

    sdbusplus::async::task<> performRemove(CMObject& cmObj) override;
    sdbusplus::async::task<> performAdd(CMObject& cmObj) override;
};

} // namespace concurrent_maintenance
