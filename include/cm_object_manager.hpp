#pragma once

#include "cm_object.hpp"

#include <sdbusplus/async/context.hpp>

#include <memory>
#include <string>

namespace concurrent_maintenance
{

/**
 * @brief Manages the creation and lifecycle of CM objects
 *
 * This class is responsible for:
 * 1. CM type determination from inventory path
 * 2. Creating objects for different types of CM (FSI, BMC, Switchboard).
 * 3. Creating differnt handlers for each type of CM, and calling them
 *    based on the CM type
 * Currently implements basic CM object creation [2].
 */
class CMObjectManager
{
  public:
    explicit CMObjectManager(sdbusplus::async::context& ctx);

    CMObjectManager(const CMObjectManager&) = delete;
    CMObjectManager& operator=(const CMObjectManager&) = delete;
    CMObjectManager(CMObjectManager&&) = delete;
    CMObjectManager& operator=(CMObjectManager&&) = delete;

    ~CMObjectManager() = default;

    /**
     * @brief Create a CM object for add or remove operation
     *
     * @param isRemove - true for remove operation, false for add operation
     * @return std::unique_ptr<CMObject> - The created CM object
     */
    std::unique_ptr<CMObject> createCMObject(bool isRemove);

  private:
    sdbusplus::async::context& ctx;
};

} // namespace concurrent_maintenance
