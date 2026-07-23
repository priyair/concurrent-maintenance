#pragma once

#include "cm_handler.hpp"
#include "cm_object.hpp"

#include <sdbusplus/async.hpp>
#include <sdbusplus/async/context.hpp>
#include <sdbusplus/async/timer.hpp>
#include <sdbusplus/bus.hpp>
#include <sdbusplus/bus/match.hpp>

#include <memory>
#include <string>

namespace concurrent_maintenance
{

/**
 * @brief Top-level CM daemon manager.
 *
 * Responsibilities (and only these):
 *   1. Watch the ReadyToRemove property change signal on all inventory
 *      objects under /xyz/openbmc_project/inventory.
 *   2. Gate-keep concurrency: drop any new request while one is already
 *      in flight.
 *   3. Call CMHandler::create() to get the right concrete handler,
 *      then call handler->execute() to drive the operation.
 *
 * Manager never inspects the FRU type; that logic lives in CMHandler.
 */
class Manager
{
  public:
    explicit Manager(sdbusplus::async::context& ctx);

    Manager(const Manager&) = delete;
    Manager& operator=(const Manager&) = delete;
    Manager(Manager&&) = delete;
    Manager& operator=(Manager&&) = delete;

    ~Manager() = default;

  private:
    sdbusplus::async::context& ctx;
    std::unique_ptr<sdbusplus::bus::match_t> readyToRemoveMatch;

    // The single in-flight handler (nullptr when idle).
    // Kept non-null for the full duration of runHandler so that any
    // concurrent handleAsync coroutine reaching manageCMHandler sees it
    // and is correctly rejected.
    std::unique_ptr<CMHandler> currentCMHandler;

    // Synchronous D-Bus callback: reads message, spawns handleAsync.
    void handleReadyToRemoveChange(sdbusplus::message_t& msg);

    // Async coroutine: yields once (sleep_for 0s) so the event loop
    // drains all pending D-Bus messages before calling manageCMHandler.
    // This ensures back-to-back signals both have their coroutines
    // spawned before either one calls manageCMHandler, so the first
    // sets currentCMHandler and the second sees it and is rejected.
    sdbusplus::async::task<> handleAsync(bool readyToRemove,
                                         std::string fruPath);

    // Gate-keep: reject if busy, create handler, spawn runHandler.
    void manageCMHandler(bool readyToRemove, const std::string& fruPath);

    // Async coroutine: co_awaits handler->execute(), then clears
    // currentCMHandler so the next operation can be accepted.
    sdbusplus::async::task<> runHandler(bool isRemove);
};

} // namespace concurrent_maintenance
