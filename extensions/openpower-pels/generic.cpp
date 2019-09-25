#include "generic.hpp"

#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace pels
{

using namespace phosphor::logging;

void Generic::unflatten(Stream& stream)
{
    stream >> _header;

    if (_header.size <= SectionHeader::flattenedSize())
    {
        throw std::out_of_range(
            "Generic::unflatten: SectionHeader::size field too small");
    }

    size_t dataLength = _header.size - SectionHeader::flattenedSize();
    _data.resize(dataLength);

    stream >> _data;
}

void Generic::flatten(Stream& stream)
{
    stream << _header << _data;
}

Generic::Generic(Stream& pel)
{
    try
    {
        unflatten(pel);
        validate();
    }
    catch (const std::exception& e)
    {
        log<level::ERR>("Cannot unflatten generic section",
                        entry("ERROR=%s", e.what()));
        _valid = false;
    }
}

void Generic::validate()
{
    // Nothing to validate
    _valid = true;
}

} // namespace pels
} // namespace openpower
