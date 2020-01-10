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
#include "user_data_formats.hpp"

#include <iomanip>
#include <nlohmann/json.hpp>
#include <phosphor-logging/log.hpp>
#include <sstream>

namespace openpower::pels::user_data
{

using namespace phosphor::logging;

/**
 * @brief Returns a JSON string for use by PEL::printSectionInJSON().
 *
 * The string must contain a JSON object, but without the outer {}.
 *
 * @param[in] json - The JSON to convert to a string
 *
 * @return std::string - The JSON string
 */
std::string prettyJSON(const nlohmann::json& json)
{
    nlohmann::json output;

    // If not a JSON object, make it one
    if (!json.is_object())
    {
        output["Data"] = json;
    }
    else
    {
        output = json;
    }

    // Let nlohmann do the pretty printing.
    std::stringstream stream;
    stream << std::setw(4) << output;

    auto jsonString = stream.str();

    // Now it looks like:
    // {
    //     "Data": ...
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

            auto json = nlohmann::json::parse(jsonString);

            return prettyJSON(json);
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
