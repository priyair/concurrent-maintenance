#include "bmc_cm_handler.hpp"

#include <phosphor-logging/lg2.hpp>

namespace concurrent_maintenance
{

sdbusplus::async::task<> BMCCMHandler::performRemove(CMObject& /*cmObj*/)
{
    lg2::info("BMCCMHandler::performRemove() for {PATH}", "PATH", fruPath);

    // Step 1: Kill FSI links to the BMC card.
    // TODO: co_await fsiAppProxy.killFSILinks(fruPath)
    // TODO: cmObj.setStep(1) once Progress interface is added.
    lg2::info("BMCCMHandler: killing FSI links for BMC at {PATH}", "PATH",
              fruPath);

    // Step 2: Delete VPD for the BMC.
    // TODO: co_await fsiAppProxy.deleteVPD(fruPath)
    // TODO: cmObj.setStep(2) once Progress interface is added.
    lg2::info("BMCCMHandler: deleting VPD for BMC at {PATH}", "PATH", fruPath);

    // Step 3: Presence-detect sets Available=false on the inventory object.
    lg2::info("BMCCMHandler: remove sequence complete for {PATH}", "PATH",
              fruPath);

    co_return;
}

sdbusplus::async::task<> BMCCMHandler::performAdd(CMObject& /*cmObj*/)
{
    lg2::info("BMCCMHandler::performAdd() for {PATH}", "PATH", fruPath);

    // Step 1: Enable FSI links to the newly inserted BMC.
    // TODO: co_await fsiAppProxy.enableFSILinks(fruPath)
    // TODO: cmObj.setStep(1) once Progress interface is added.
    lg2::info("BMCCMHandler: enabling FSI links for BMC at {PATH}", "PATH",
              fruPath);

    // Step 2: Collect VPD of the new BMC.
    // TODO: co_await fsiAppProxy.collectVPD(fruPath)
    // TODO: cmObj.setStep(2) once Progress interface is added.
    lg2::info("BMCCMHandler: collecting VPD for BMC at {PATH}", "PATH",
              fruPath);

    // Step 3: Presence-detect sets Available=true once the BMC is enumerated.
    lg2::info("BMCCMHandler: add sequence complete for {PATH}", "PATH",
              fruPath);

    co_return;
}

} // namespace concurrent_maintenance
