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
