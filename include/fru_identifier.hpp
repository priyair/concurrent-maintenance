#pragma once

#include <string>

namespace concurrent_maintenance
{

/**
 * @brief FRU types supported for Concurrent Maintenance.
 */
enum class FRUType
{
    FSI,         ///< FSI card (PCIe card with FSI interface)
    BMC,         ///< Baseboard Management Controller card
    SWITCHBOARD, ///< Switchboard
    UNKNOWN      ///< Unrecognised / unsupported FRU
};

/**
 * @brief Identifies FRU type from an inventory D-Bus object path.
 *
 * Used exclusively by CMObjectManager; Manager never needs to know the type.
 */
class FRUIdentifier
{
  public:
    /**
     * @brief Determine the FRU type from its inventory path.
     *
     * @param objectPath  e.g.
     *   /xyz/openbmc_project/inventory/system/chassis/motherboard/fsi_card0
     * @return FRUType
     */
    static FRUType identifyType(const std::string& objectPath);

    /** @brief Human-readable string for a FRUType value. */
    static std::string typeToString(FRUType type);
};

} // namespace concurrent_maintenance
