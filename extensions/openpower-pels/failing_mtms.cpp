/**
 * Copyright © 2019 IBM Corporation
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
#include "failing_mtms.hpp"

#include "json_utils.hpp"
#include "pel_types.hpp"
#include "pel_values.hpp"

#include <fmt/format.h>

#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace pels
{

namespace pv = openpower::pels::pel_values;
using namespace phosphor::logging;
static constexpr uint8_t failingMTMSVersion = 0x01;

FailingMTMS::FailingMTMS(const DataInterfaceBase& dataIface) :
    _mtms(dataIface.getMachineTypeModel(), dataIface.getMachineSerialNumber())
{
    _header.id = static_cast<uint16_t>(SectionID::failingMTMS);
    _header.size = FailingMTMS::flattenedSize();
    _header.version = failingMTMSVersion;
    _header.subType = 0;
    _header.componentID = static_cast<uint16_t>(ComponentID::phosphorLogging);

    _valid = true;
}

FailingMTMS::FailingMTMS(Stream& pel)
{
    try
    {
        unflatten(pel);
        validate();
    }
    catch (const std::exception& e)
    {
        log<level::ERR>(
            fmt::format("Cannot unflatten failing MTM section: {}", e.what())
                .c_str());
        _valid = false;
    }
}

void FailingMTMS::validate()
{
    bool failed = false;

    if (header().id != static_cast<uint16_t>(SectionID::failingMTMS))
    {
        log<level::ERR>(
            fmt::format("Invalid failing MTMS section ID: {0:#x}", header().id)
                .c_str());
        failed = true;
    }

    if (header().version != failingMTMSVersion)
    {
        log<level::ERR>(fmt::format("Invalid failing MTMS version: {0:#x}",
                                    header().version)
                            .c_str());
        failed = true;
    }

    _valid = (failed) ? false : true;
}

void FailingMTMS::flatten(Stream& stream) const
{
    stream << _header << _mtms;
}

void FailingMTMS::unflatten(Stream& stream)
{
    stream >> _header >> _mtms;
}

std::optional<std::string> FailingMTMS::getJSON(uint8_t creatorID) const
{
    std::string json;
    jsonInsert(json, pv::sectionVer, getNumberString("%d", _header.version), 1);
    jsonInsert(json, pv::subSection, getNumberString("%d", _header.subType), 1);
    jsonInsert(json, pv::createdBy,
               getComponentName(_header.componentID, creatorID), 1);
    jsonInsert(json, "Machine Type Model", _mtms.machineTypeAndModel(), 1);
    jsonInsert(json, "Serial Number", trimEnd(_mtms.machineSerialNumber()), 1);
    json.erase(json.size() - 2);
    return json;
}
} // namespace pels
} // namespace openpower
