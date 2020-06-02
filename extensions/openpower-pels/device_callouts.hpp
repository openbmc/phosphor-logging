#pragma once

#include "pel_types.hpp"

#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>
#include <tuple>
#include <vector>

/**
 * @file device_callouts.hpp
 *
 * @brief Looks up FRU callouts, which are D-Bus inventory paths,
 * in JSON for device sysfs paths.
 *
 * The code will extract the search keys from the sysfs path to
 * use to look up the callout list in the JSON.  The callouts will
 * be sorted by priority as defined in th PEL spec.
 *
 * The JSON is normally generated from the MRW, and contains
 * sections for the following types of callouts:
 *   * I2C  (also based on bus/addr as well as the path)
 *   * FSI
 *   * FSI-I2C
 *   * FSI-SPI
 *
 * The JSON looks like:
 *
 * "I2C":
 *   "<bus>":
 *     "<address>":
 *       "Callouts": [
 *          {
 *             "LocationCode": "<location code>",
 *             "Name": "<inventory path>",
 *             "Priority": "<priority=H/M/L>",
 *             "MRU": "<optional MRU name>"
 *          },
 *          ...
 *       ],
 *       "Dest": "<destination MRW target>"
 *
 * "FSI":
 *   "<fsi link>":
 *     "Callouts": [
 *            ...
 *     ],
 *     "Dest": "<destination MRW target>"
 *
 * "FSI-I2C":
 *    "<fsi link>":
 *      "<bus>":
 *        "<address>":
 *            "Callouts": [
 *                   ...
 *             ],
 *            "Dest": "<destination MRW target>"
 *
 * "FSI-SPI":
 *    "<fsi link>":
 *      "<bus>":
 *         "Callouts": [
 *                ...
 *         ],
 *         "Dest": "<destination MRW target>"
 *
 */

namespace openpower::pels::device_callouts
{

/**
 * @brief Represents a callout in the device JSON.
 *
 * The debug field will only be filled in for the first
 * callout in the list of them and contains additional
 * information about what happened when looking up the
 * callouts that is meant to aid in debug.
 */
struct Callout
{
    std::string priority;
    std::string locationCode;
    std::string name;
    std::string mru;
    std::string debug;
};

/**
 * @brief Looks up the callouts in a JSON File to add to a PEL
 *        for when the path between the BMC and the device specified
 *        by the passed in device path needs to be called out.
 *
 * The path is the path used to access the device in sysfs.  It
 * can be either a directory path or a file path.
 *
 * @param[in] devPath - The device path
 * @param[in] compatibleList - The list of compatible names for this
 *                             system.
 * @return std::vector<Callout> - The list of callouts
 */
std::vector<Callout>
    getCallouts(const std::string& devPath,
                const std::vector<std::string>& compatibleList);

/**
 * @brief Looks up the callouts to add to a PEL for when the path
 *        between the BMC and the device specified by the passed in
 *        I2C bus and address needs to be called out.
 *
 * @param[in] i2cBus - The I2C bus
 * @param[in] i2cAddress - The I2C address
 * @param[in] compatibleList - The list of compatible names for this
 *                             system.
 * @return std::vector<Callout> - The list of callouts
 */
std::vector<Callout>
    getI2CCallouts(size_t i2cBus, uint8_t i2cAddress,
                   const std::vector<std::string>& compatibleList);

namespace util
{

/**
 * @brief Returns the path to the JSON file to look up callouts in.
 *
 * @param[in] compatibleList - The list of compatible names for this
 *                             system.
 *
 * @return path - The path to the file.
 */
std::filesystem::path
    getJSONFilename(const std::vector<std::string>& compatibleList);

/**
 * @brief Looks up the callouts in the JSON using the I2C keys.
 *
 * @param[in] i2cBus - The I2C bus
 * @param[in] i2cAddress - The I2C address
 * @param[in] calloutJSON - The JSON containing the callouts
 *
 * @return std::vector<Callout> - The callouts
 */
std::vector<device_callouts::Callout>
    calloutI2C(size_t i2CBus, uint8_t i2cAddress,
               const nlohmann::json& calloutJSON);
} // namespace util
} // namespace openpower::pels::device_callouts
