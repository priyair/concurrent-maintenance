#pragma once

#include "cm_object.hpp"
#include "cm_object_manager.hpp"

#include <sdbusplus/async/context.hpp>
#include <sdbusplus/async/match.hpp>

#include <memory>
#include <string>

namespace concurrent_maintenance
{

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

    // "ReadyToRemove" Property change signal match
    std::unique_ptr<sdbusplus::async::match> readyToRemoveMatch;

    // CM object manager
    CMObjectManager cmObjectManager;

    // Current CM object
    std::unique_ptr<CMObject> currentCMObject;

    // Callback for ReadyToRemove property change
    void handleReadyToRemoveChange(sdbusplus::message_t& msg);

    // Create/remove CM object based on ReadyToRemove value
    void manageCMObject(bool readyToRemove);
};

} // namespace concurrent_maintenance
