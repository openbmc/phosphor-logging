#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace phosphor::logging::extensions::ael
{

/**
 * @brief Event-specific AFID metadata.
 *
 * This structure contains metadata associated with a
 * particular event entry resolved through the generated
 * reverse lookup table.
 */
struct AFIDInfo
{
    /**
     * @brief AMD Field Identifier.
     */
    uint64_t afid{};

    /**
     * @brief Inventory objects associated with the event.
     *
     * Derived from the originOfCondition entries in the
     * reverse lookup table.
     */
    std::vector<std::string> origins;

    /**
     * @brief Associated Redfish resource paths.
     */
    std::vector<std::string> redfishMappings;

    /**
     * @brief Human-readable event description.
     */
    std::string description;
};

/**
 * @brief Static metadata exported by the generated LUT.
 *
 * These values are global to the platform and are not
 * associated with any particular event.
 */
struct AFIDStaticInfo
{
    /**
     * @brief LUT version.
     */
    std::string version;

    /**
     * @brief Fallback AFID returned when lookup resolution
     * does not reach a leaf entry.
     */
    uint64_t fallthroughAFID{};

    /**
     * @brief Inventory object representing the rack unit.
     */
    std::string rackUnitPosition;

    /**
     * @brief Redfish schema supported by the platform.
     */
    std::string schema;
};

/**
 * @brief Resolve event metadata to an AFID entry.
 *
 * Uses the generated reverse lookup table to match an
 * event message and associated additional data against
 * the configured AMD Event Log metadata.
 *
 * @param[in] message Event message identifier.
 * @param[in] addData Event additional data.
 *
 * @return Resolved AFID metadata.
 */
AFIDInfo lookupAFID(const std::string& message,
                    const std::map<std::string, std::string>& addData);

/**
 * @brief Retrieve static metadata from the generated LUT.
 *
 * Returns metadata defined at the top level of the LUT
 * definition and independent of any specific event.
 *
 * @return Static LUT metadata.
 */
AFIDStaticInfo getStaticData();

} // namespace phosphor::logging::extensions::ael
