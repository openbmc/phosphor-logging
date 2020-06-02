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
#include "device_callouts.hpp"

#include "paths.hpp"

#include <fstream>
#include <phosphor-logging/log.hpp>
#include <regex>

namespace openpower::pels::device_callouts
{

constexpr auto debugFilePath = "/etc/phosphor-logging/";
constexpr auto calloutFileSuffix = "_dev_callouts.json";

namespace fs = std::filesystem;
using namespace phosphor::logging;

namespace util
{

fs::path getJSONFilename(const std::vector<std::string>& compatibleList)
{
    auto basePath = getPELReadOnlyDataPath();
    fs::path fullPath;

    // Find an entry in the list of compatible system names that
    // matches a filename we have.

    for (const auto& name : compatibleList)
    {
        fs::path filename = name + calloutFileSuffix;

        // Check the debug path first
        fs::path path{fs::path{debugFilePath} / filename};

        if (fs::exists(path))
        {
            log<level::INFO>("Found device callout debug file");
            fullPath = path;
            break;
        }

        path = basePath / filename;

        if (fs::exists(path))
        {
            fullPath = path;
            break;
        }
    }

    if (fullPath.empty())
    {
        throw std::invalid_argument(
            "No JSON dev path callout file for this system");
    }

    return fullPath;
}

/**
 * @brief Reads the callout JSON into an object based on the
 *        compatible system names list.
 *
 * @param[in] compatibleList - The list of compatible names for this
 *                             system.
 *
 * @return nlohmann::json - The JSON object
 */
nlohmann::json loadJSON(const std::vector<std::string>& compatibleList)
{
    auto filename = getJSONFilename(compatibleList);
    std::ifstream file{filename};
    return nlohmann::json::parse(file);
}

std::vector<device_callouts::Callout>
    calloutI2C(int errnoValue, size_t i2cBus, uint8_t i2cAddress,
               const nlohmann::json& calloutJSON)
{
    // TODO
    return {};
}

std::vector<device_callouts::Callout> findCallouts(int errnoValue,
                                                   const std::string& devPath,
                                                   const nlohmann::json& json)
{
    std::vector<Callout> callouts;

    // TODO

    return callouts;
}

} // namespace util

std::vector<Callout> getCallouts(int errnoValue, const std::string& devPath,
                                 const std::vector<std::string>& compatibleList)
{
    auto json = util::loadJSON(compatibleList);
    return util::findCallouts(errnoValue, devPath, json);
}

std::vector<Callout>
    getI2CCallouts(int errnoValue, size_t i2cBus, uint8_t i2cAddress,
                   const std::vector<std::string>& compatibleList)
{
    auto json = util::loadJSON(compatibleList);
    return util::calloutI2C(errnoValue, i2cBus, i2cAddress, json);
}

} // namespace openpower::pels::device_callouts
