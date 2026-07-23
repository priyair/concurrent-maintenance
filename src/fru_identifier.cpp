#include "fru_identifier.hpp"

#include <algorithm>
#include <cctype>
#include <string>

namespace concurrent_maintenance
{

FRUType FRUIdentifier::identifyType(const std::string& objectPath)
{
    // Extract the last path segment (everything after the final "/").
    // Matching on the last segment only avoids false positives where a
    // keyword like "fsi" or "bmc" appears in a parent directory name
    // rather than the FRU node itself.
    //   e.g. ".../fsi_chassis/motherboard/cpu0" must NOT match FSI.
    auto pos = objectPath.rfind('/');
    std::string segment =
        (pos != std::string::npos) ? objectPath.substr(pos + 1) : objectPath;

    // Case-insensitive match against the last segment only.
    std::string lowerSeg = segment;
    std::transform(lowerSeg.begin(), lowerSeg.end(), lowerSeg.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    // FSI card: segment contains "fsi" or "pcie_card" / "pcie-card".
    // Examples:
    //   .../motherboard/fsi_card0   -> segment = "fsi_card0"
    //   .../motherboard/pcie_card1  -> segment = "pcie_card1"
    if (lowerSeg.find("fsi") != std::string::npos ||
        lowerSeg.find("pcie_card") != std::string::npos ||
        lowerSeg.find("pcie-card") != std::string::npos)
    {
        return FRUType::FSI;
    }

    // BMC: segment equals "bmc", ends with "_bmc", or contains "ebmc".
    // Examples:
    //   .../motherboard/bmc         -> segment = "bmc"
    //   .../motherboard/ebmc_card   -> segment = "ebmc_card"
    if (lowerSeg == "bmc" || lowerSeg.find("_bmc") != std::string::npos ||
        lowerSeg.find("ebmc") != std::string::npos)
    {
        return FRUType::BMC;
    }

    // Switchboard: segment contains "switchboard", "switch_board", or
    // "switch-board".
    // Examples:
    //   .../chassis/switchboard0    -> segment = "switchboard0"
    //   .../chassis/switch_board0   -> segment = "switch_board0"
    if (lowerSeg.find("switchboard") != std::string::npos ||
        lowerSeg.find("switch_board") != std::string::npos ||
        lowerSeg.find("switch-board") != std::string::npos)
    {
        return FRUType::SWITCHBOARD;
    }

    return FRUType::UNKNOWN;
}

std::string FRUIdentifier::typeToString(FRUType type)
{
    switch (type)
    {
        case FRUType::FSI:
            return "FSI";
        case FRUType::BMC:
            return "BMC";
        case FRUType::SWITCHBOARD:
            return "SWITCHBOARD";
        case FRUType::UNKNOWN:
        default:
            return "UNKNOWN";
    }
}

} // namespace concurrent_maintenance
