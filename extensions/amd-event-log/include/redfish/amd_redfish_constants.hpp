#pragma once

#include <array>
#include <cstdint>
#include <string_view>

namespace phosphor::logging::extensions::ael::redfish::constants
{

/*
 * TODO:
 * Replace static platform mappings with build-time
 * generated metadata. Future implementations should
 * derive AFID ownership, schema identifiers, and
 * projection rules from platform-specific sources.
 */

/**
 * @brief AFID range description mapping.
 */
struct AFIDRange
{
    /** First AFID in the range. */
    uint64_t start{};

    /** Last AFID in the range. */
    uint64_t end{};

    /** Description associated with the AFID range. */
    std::string_view description;
};

/**
 * @brief AFID ownership ranges.
 */
inline constexpr std::array<AFIDRange, 7> afidRanges{{
    {11000, 11999, "EAM AMC Error"},
    {12000, 12999, "Compute Tray Error"},
    {13000, 13999, "Switch Tray Error"},
    {14000, 14999, "Rack Error"},
    {15000, 16999, "Compute Tray Tools Error"},
    {20000, 39999, "CPU CPER Error"},
    {40000, 59999, "GPU CPER Error"},
}};

/**
 * @brief AMD OEM schema identifier.
 */
inline constexpr std::string_view messageOdataType =
    "#AMD_Message.v1_0_0.AMD_Message";

} // namespace phosphor::logging::extensions::ael::redfish::constants
