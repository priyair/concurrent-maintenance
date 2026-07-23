#include "fsi_card_cm_handler.hpp"

#include <phosphor-logging/lg2.hpp>

namespace concurrent_maintenance
{

sdbusplus::async::task<> FSICardCMHandler::performRemove(CMObject& /*cmObj*/)
{
    lg2::info("FSICardCMHandler::performRemove() for {PATH}", "PATH", fruPath);

    // Step 1: Kill FSI links to this card.
    // TODO: co_await fsiAppProxy.killFSILinks(fruPath)
    // TODO: cmObj.setStep(1) once Progress interface is added.
    lg2::info("FSICardCMHandler: killing FSI links for {PATH}", "PATH",
              fruPath);

    // Step 2: Delete VPD for this card.
    // TODO: co_await fsiAppProxy.deleteVPD(fruPath)
    // TODO: cmObj.setStep(2) once Progress interface is added.
    lg2::info("FSICardCMHandler: deleting VPD for {PATH}", "PATH", fruPath);

    // Step 3: Presence-detect (FSI app) sets Available=false on the
    // inventory object in response to the Kill command above.
    lg2::info("FSICardCMHandler: remove sequence complete for {PATH}", "PATH",
              fruPath);

    co_return;
}

sdbusplus::async::task<> FSICardCMHandler::performAdd(CMObject& /*cmObj*/)
{
    lg2::info("FSICardCMHandler::performAdd() for {PATH}", "PATH", fruPath);

    // Step 1: Enable FSI links to the newly inserted card.
    // TODO: co_await fsiAppProxy.enableFSILinks(fruPath)
    // TODO: cmObj.setStep(1) once Progress interface is added.
    lg2::info("FSICardCMHandler: enabling FSI links for {PATH}", "PATH",
              fruPath);

    // Step 2: Collect VPD of the new card.
    // TODO: co_await fsiAppProxy.collectVPD(fruPath)
    // TODO: cmObj.setStep(2) once Progress interface is added.
    lg2::info("FSICardCMHandler: collecting VPD for {PATH}", "PATH", fruPath);

    // Step 3: Presence-detect (FSI app) sets Available=true once
    // the card is enumerated.
    lg2::info("FSICardCMHandler: add sequence complete for {PATH}", "PATH",
              fruPath);

    co_return;
}

} // namespace concurrent_maintenance
