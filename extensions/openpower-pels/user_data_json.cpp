/**
 * Copyright © 2020 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "user_data_json.hpp"

#include "pel_types.hpp"
#include "pel_values.hpp"
#include "user_data_formats.hpp"

#include <fifo_map.hpp>
#include <iomanip>
#include <nlohmann/json.hpp>
#include <phosphor-logging/log.hpp>
#include <sstream>

namespace openpower::pels::user_data
{
namespace pv = openpower::pels::pel_values;
using namespace phosphor::logging;

// Use fifo_map as nlohmann::json's map. We are just ignoring the 'less'
// compare.  With this map the keys are kept in FIFO order.
template <class K, class V, class dummy_compare, class A>
using fifoMap = nlohmann::fifo_map<K, V, nlohmann::fifo_map_compare<K>, A>;
using fifoJSON = nlohmann::basic_json<fifoMap>;

/**
 * @brief Returns a JSON string for use by PEL::printSectionInJSON().
 *
 * The returning string will contain a JSON object, but without
 * the outer {}.  If the input JSON isn't a JSON object (dict), then
 * one will be created with the input added to a 'Data' key.
 *
 * @param[in] json - The JSON to convert to a string
 *
 * @return std::string - The JSON string
 */
std::string prettyJSON(uint16_t componentID, uint8_t subType, uint8_t version,
                       const fifoJSON& json)
{
    fifoJSON output;
    output[pv::sectionVer] = std::to_string(version);
    output[pv::subSection] = std::to_string(subType);

    char value[10];
    sprintf(value, "0x%04X", componentID);
    output[pv::createdBy] = std::string{value};

    if (!json.is_object())
    {
        output["Data"] = json;
    }
    else
    {
        for (const auto& [key, value] : json.items())
        {
            output[key] = value;
        }
    }

    // Let nlohmann do the pretty printing.
    std::stringstream stream;
    stream << std::setw(4) << output;

    auto jsonString = stream.str();

    // Now it looks like:
    // {
    //     "Section Version": ...
    //     ...
    // }

    // Since PEL::printSectionInJSON() will supply the outer { }s,
    // remove the existing ones.

    // Replace the { and the following newline, and the } and its
    // preceeding newline.
    jsonString.erase(0, 2);

    auto pos = jsonString.find_last_of('}');
    jsonString.erase(pos - 1);

    return jsonString;
}

/**
 * @brief Convert to an appropriate JSON string as the data is one of
 *        the formats that we natively support.
 *
 * @param[in] componentID - The comp ID from the UserData section header
 * @param[in] subType - The subtype from the UserData section header
 * @param[in] version - The version from the UserData section header
 * @param[in] data - The data itself
 *
 * @return std::optional<std::string> - The JSON string if it could be created,
 *                                      else std::nullopt.
 */
std::optional<std::string>
    getBuiltinFormatJSON(uint16_t componentID, uint8_t subType, uint8_t version,
                         const std::vector<uint8_t>& data)
{
    switch (subType)
    {
        case static_cast<uint8_t>(UserDataFormat::json):
        {
            std::string jsonString{data.begin(), data.begin() + data.size()};

            fifoJSON json = nlohmann::json::parse(jsonString);

            return prettyJSON(componentID, subType, version, json);
        }
        default:
            break;
    }
    return std::nullopt;
}

std::optional<std::string> getJSON(uint16_t componentID, uint8_t subType,
                                   uint8_t version,
                                   const std::vector<uint8_t>& data)
{
    try
    {
        switch (componentID)
        {
            case static_cast<uint16_t>(ComponentID::phosphorLogging):
                return getBuiltinFormatJSON(componentID, subType, version,
                                            data);
            default:
                break;
        }
    }
    catch (std::exception& e)
    {
        log<level::ERR>("Failed parsing UserData", entry("ERROR=%s", e.what()));
    }

    return std::nullopt;
}

} // namespace openpower::pels::user_data
