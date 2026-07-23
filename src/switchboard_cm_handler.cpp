#include "switchboard_cm_handler.hpp"

#include <phosphor-logging/lg2.hpp>

namespace concurrent_maintenance
{

sdbusplus::async::task<>
    SwitchboardCMHandler::performRemove(CMObject& /*cmObj*/)
{
    lg2::info("SwitchboardCMHandler::performRemove() for {PATH}", "PATH",
              fruPath);

    // Step 1: Kill FSI links to the switchboard.
    // TODO: co_await fsiAppProxy.killFSILinks(fruPath)
    // TODO: cmObj.setStep(1) once Progress interface is added.
    lg2::info("SwitchboardCMHandler: killing FSI links for {PATH}", "PATH",
              fruPath);

    // Step 2: Delete VPD for the switchboard.
    // TODO: co_await fsiAppProxy.deleteVPD(fruPath)
    // TODO: cmObj.setStep(2) once Progress interface is added.
    lg2::info("SwitchboardCMHandler: deleting VPD for {PATH}", "PATH", fruPath);

    // Step 3: Presence-detect sets Available=false on the inventory object.
    lg2::info("SwitchboardCMHandler: remove sequence complete for {PATH}",
              "PATH", fruPath);

    co_return;
}

sdbusplus::async::task<> SwitchboardCMHandler::performAdd(CMObject& /*cmObj*/)
{
    lg2::info("SwitchboardCMHandler::performAdd() for {PATH}", "PATH", fruPath);

    // Step 1: Enable FSI links to the newly inserted switchboard.
    // TODO: co_await fsiAppProxy.enableFSILinks(fruPath)
    // TODO: cmObj.setStep(1) once Progress interface is added.
    lg2::info("SwitchboardCMHandler: enabling FSI links for {PATH}", "PATH",
              fruPath);

    // Step 2: Collect VPD of the new switchboard.
    // TODO: co_await fsiAppProxy.collectVPD(fruPath)
    // TODO: cmObj.setStep(2) once Progress interface is added.
    lg2::info("SwitchboardCMHandler: collecting VPD for {PATH}", "PATH",
              fruPath);

    // Step 3: Presence-detect sets Available=true once enumerated.
    lg2::info("SwitchboardCMHandler: add sequence complete for {PATH}", "PATH",
              fruPath);

    co_return;
}

} // namespace concurrent_maintenance
