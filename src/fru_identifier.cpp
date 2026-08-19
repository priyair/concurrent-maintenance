// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright IBM Corp.

#include "fru_identifier.hpp"

#include "cm_handlers.hpp"

#include <algorithm>
#include <array>
#include <string>
#include <string_view>
#include <vector>

namespace concurrent_maintenance
{

namespace
{

/** Returns true if 'interfaces' contains 'iface'. */
bool hasInterface(const std::vector<std::string>& interfaces,
                  std::string_view iface)
{
    return std::ranges::find(interfaces, iface) != interfaces.end();
}

bool matchBmcCard(const std::vector<std::string>& interfaces,
                  const std::string& /*fruPath*/)
{
    /* ebmc_card: hosts both Item.Board and Decorator.Asset.
     * mcm hosts Decorator.Asset but Item.Cpu, not Item.Board. */
    return hasInterface(interfaces,
                        "xyz.openbmc_project.Inventory.Decorator.Asset") &&
           hasInterface(interfaces, "xyz.openbmc_project.Inventory.Item.Board");
}

bool matchFsiCard(const std::vector<std::string>& interfaces,
                  const std::string& fruPath)
{
    /* fsi_card: hosts Item.Board but NOT Decorator.Asset. */
    return hasInterface(interfaces,
                        "xyz.openbmc_project.Inventory.Item.Board") &&
           !hasInterface(interfaces,
                         "xyz.openbmc_project.Inventory.Decorator.Asset") &&
           fruPath.find("fsi") != std::string::npos;
}

bool matchSwitchboard(const std::vector<std::string>& interfaces,
                      const std::string& fruPath)
{
    /* Use Item.Board.Motherboard when the mapper has the object;
     * fall back to path matching for systems where switchboard is
     * not yet present in inventory. */
    if (!interfaces.empty())
    {
        return hasInterface(
            interfaces, "xyz.openbmc_project.Inventory.Item.Board.Motherboard");
    }
    return fruPath.find("switchboard") != std::string::npos;
}

const std::array<FRUEntry, 3> handlerTable = {{
    {matchBmcCard, {bmcRemove, bmcAdd}},
    {matchFsiCard, {fsiCardRemove, fsiCardAdd}},
    {matchSwitchboard, {switchboardRemove, switchboardAdd}},
}};

} // namespace

const FRUOperations*
    FRUIdentifier::identifyType(const std::vector<std::string>& interfaces,
                                const std::string& fruPath)
{
    for (const auto& entry : handlerTable)
    {
        if (entry.match(interfaces, fruPath))
        {
            return &entry.ops;
        }
    }
    return nullptr;
}

} // namespace concurrent_maintenance
