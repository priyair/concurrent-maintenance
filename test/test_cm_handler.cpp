#include "bmc_cm_handler.hpp"
#include "cm_handler.hpp"
#include "fsi_card_cm_handler.hpp"
#include "switchboard_cm_handler.hpp"

#include <sdbusplus/async/context.hpp>
#include <sdbusplus/async/execution.hpp>

#include <gtest/gtest.h>

namespace concurrent_maintenance
{

static const std::string kFSIPath =
    "/xyz/openbmc_project/inventory/system/chassis/motherboard/fsi_card0";
static const std::string kBMCPath =
    "/xyz/openbmc_project/inventory/system/chassis/motherboard/bmc";
static const std::string kSWPath =
    "/xyz/openbmc_project/inventory/system/chassis/switchboard0";
static const std::string kUnknownPath =
    "/xyz/openbmc_project/inventory/system/chassis/motherboard/cpu0";

// CMHandler::create() factory

TEST(CMHandlerFactoryTest, CreateReturnsFSICardCMHandlerForFSIPath)
{
    sdbusplus::async::context ctx;
    auto h = CMHandler::create(ctx, kFSIPath);
    ASSERT_NE(h, nullptr);
    EXPECT_EQ(h->getFruPath(), kFSIPath);
}

TEST(CMHandlerFactoryTest, CreateReturnsBMCCMHandlerForBMCPath)
{
    sdbusplus::async::context ctx;
    auto h = CMHandler::create(ctx, kBMCPath);
    ASSERT_NE(h, nullptr);
    EXPECT_EQ(h->getFruPath(), kBMCPath);
}

TEST(CMHandlerFactoryTest, CreateReturnsSwitchboardCMHandlerForSwitchboardPath)
{
    sdbusplus::async::context ctx;
    auto h = CMHandler::create(ctx, kSWPath);
    ASSERT_NE(h, nullptr);
    EXPECT_EQ(h->getFruPath(), kSWPath);
}

TEST(CMHandlerFactoryTest, CreateReturnsNullptrForUnknownPath)
{
    sdbusplus::async::context ctx;
    auto h = CMHandler::create(ctx, kUnknownPath);
    EXPECT_EQ(h, nullptr);
}

// createCMObject()

TEST(CMHandlerTest, CreateCMObjectRemovePath)
{
    sdbusplus::async::context ctx;
    auto h = CMHandler::create(ctx, kFSIPath);
    ASSERT_NE(h, nullptr);
    auto obj = h->createCMObject(true);
    ASSERT_NE(obj, nullptr);
    EXPECT_EQ(obj->getPath(), "/com/ibm/concurrent_maintenance/remove");
}

TEST(CMHandlerTest, CreateCMObjectAddPath)
{
    sdbusplus::async::context ctx;
    auto h = CMHandler::create(ctx, kBMCPath);
    ASSERT_NE(h, nullptr);
    auto obj = h->createCMObject(false);
    ASSERT_NE(obj, nullptr);
    EXPECT_EQ(obj->getPath(), "/com/ibm/concurrent_maintenance/add");
}

/* performRemove / performAdd
 *
 * stdexec::sync_wait() drives a task<> coroutine to completion synchronously.
 * It is the correct pattern for unit-testing task<> bodies: it initialises
 * the promise scheduler before the first co_await, which is what ctx.spawn()
 * via a runTask wrapper failed to guarantee, causing the clang-analyzer
 * "Branch condition evaluates to a garbage value" on __context_ in
 * __optional::operator*.
 */

TEST(FSICardCMHandlerTest, PerformRemoveExecutes)
{
    sdbusplus::async::context ctx;
    FSICardCMHandler h(ctx, kFSIPath);
    CMObject cmObj(ctx, "/com/ibm/concurrent_maintenance/remove");
    EXPECT_NO_THROW(stdexec::sync_wait(h.performRemove(cmObj)));
}

TEST(FSICardCMHandlerTest, PerformAddExecutes)
{
    sdbusplus::async::context ctx;
    FSICardCMHandler h(ctx, kFSIPath);
    CMObject cmObj(ctx, "/com/ibm/concurrent_maintenance/add");
    EXPECT_NO_THROW(stdexec::sync_wait(h.performAdd(cmObj)));
}

TEST(BMCCMHandlerTest, PerformRemoveExecutes)
{
    sdbusplus::async::context ctx;
    BMCCMHandler h(ctx, kBMCPath);
    CMObject cmObj(ctx, "/com/ibm/concurrent_maintenance/remove");
    EXPECT_NO_THROW(stdexec::sync_wait(h.performRemove(cmObj)));
}

TEST(BMCCMHandlerTest, PerformAddExecutes)
{
    sdbusplus::async::context ctx;
    BMCCMHandler h(ctx, kBMCPath);
    CMObject cmObj(ctx, "/com/ibm/concurrent_maintenance/add");
    EXPECT_NO_THROW(stdexec::sync_wait(h.performAdd(cmObj)));
}

TEST(SwitchboardCMHandlerTest, PerformRemoveExecutes)
{
    sdbusplus::async::context ctx;
    SwitchboardCMHandler h(ctx, kSWPath);
    CMObject cmObj(ctx, "/com/ibm/concurrent_maintenance/remove");
    EXPECT_NO_THROW(stdexec::sync_wait(h.performRemove(cmObj)));
}

TEST(SwitchboardCMHandlerTest, PerformAddExecutes)
{
    sdbusplus::async::context ctx;
    SwitchboardCMHandler h(ctx, kSWPath);
    CMObject cmObj(ctx, "/com/ibm/concurrent_maintenance/add");
    EXPECT_NO_THROW(stdexec::sync_wait(h.performAdd(cmObj)));
}

/* Polymorphic dispatch via execute() */

TEST(CMHandlerTest, PolymorphicRemoveViaExecute)
{
    sdbusplus::async::context ctx;
    auto h = CMHandler::create(ctx, kFSIPath);
    ASSERT_NE(h, nullptr);
    EXPECT_NO_THROW(stdexec::sync_wait(h->execute(true)));
}

TEST(CMHandlerTest, PolymorphicAddViaExecute)
{
    sdbusplus::async::context ctx;
    auto h = CMHandler::create(ctx, kBMCPath);
    ASSERT_NE(h, nullptr);
    EXPECT_NO_THROW(stdexec::sync_wait(h->execute(false)));
}

} // namespace concurrent_maintenance
