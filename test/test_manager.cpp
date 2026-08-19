// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright IBM Corp.

#include "manager.hpp"

#include <sdbusplus/async.hpp>

#include <gtest/gtest.h>

namespace concurrent_maintenance
{

TEST(ManagerTest, CanBeConstructed)
{
    sdbusplus::async::context ctx;
    Manager manager(ctx);
    ctx.spawn(stdexec::just() |
              stdexec::then([&ctx]() { ctx.request_stop(); }));
    ctx.run();
}

} // namespace concurrent_maintenance
