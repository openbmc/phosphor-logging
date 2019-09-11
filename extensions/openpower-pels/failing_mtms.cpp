#include "failing_mtms.hpp"

#include "pel_types.hpp"

#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace pels
{

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
}

FailingMTMS::FailingMTMS(Stream& pel)
{
    try
    {
        unflatten(pel);
        validate();
    }
    catch (std::exception& e)
    {
        log<level::ERR>("Cannot unflatten failing MTM section",
                        entry("ERROR=%s", e.what()));
        _valid = false;
    }
}

void FailingMTMS::validate()
{
    bool failed = false;

    if (header().id != static_cast<uint16_t>(SectionID::failingMTMS))
    {
        log<level::ERR>("Invalid failing MTMS section ID",
                        entry("ID=0x%X", header().id));
        failed = true;
    }

    if (header().version != failingMTMSVersion)
    {
        log<level::ERR>("Invalid failing MTMS version",
                        entry("VERSION=0x%X", header().version));
        failed = true;
    }

    _valid = (failed) ? false : true;
}

std::string FailingMTMS::getMachineTypeModel()
{
    std::string mtmString;
    const auto& mtm = _mtms.machineTypeAndModel();

    for (size_t i = 0; (i < MTMS::mtmSize) && (mtm[i] != 0); i++)
    {
        mtmString.push_back(mtm[i]);
    }
    return mtmString;
}

std::string FailingMTMS::getMachineSerialNumber()
{
    std::string snString;
    const auto& sn = _mtms.machineSerialNumber();

    // Get everything up to the 0s.
    for (size_t i = 0; (i < MTMS::snSize) && (sn[i] != 0); i++)
    {
        snString.push_back(sn[i]);
    }
    return snString;
}

void FailingMTMS::flatten(Stream& stream)
{
    stream << _header << _mtms;
}

void FailingMTMS::unflatten(Stream& stream)
{
    stream >> _header >> _mtms;
}

} // namespace pels
} // namespace openpower
