// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "failing_mtms.hpp"

#include "json_utils.hpp"
#include "pel_types.hpp"
#include "pel_values.hpp"

#include <phosphor-logging/lg2.hpp>

namespace openpower
{
namespace pels
{

namespace pv = openpower::pels::pel_values;

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
        lg2::error("Cannot unflatten failing MTM section: {EXCEPTION}",
                   "EXCEPTION", e);
        _valid = false;
    }
}

void FailingMTMS::validate()
{
    bool failed = false;

    if (header().id != static_cast<uint16_t>(SectionID::failingMTMS))
    {
        lg2::error("Invalid failing MTMS section ID: {HEADER_ID}", "HEADER_ID",
                   lg2::hex, header().id);
        failed = true;
    }

    if (header().version != failingMTMSVersion)
    {
        lg2::error("Invalid failing MTMS version: {HEADER_VERSION}",
                   "HEADER_VERSION", lg2::hex, header().version);
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
