#include "src.hpp"

#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace pels
{

using namespace phosphor::logging;

void SRC::unflatten(Stream& stream)
{
    stream >> _header >> _version >> _flags >> _reserved1B >> _wordCount >>
        _reserved2B >> _size;

    for (auto& word : _hexData)
    {
        stream >> word;
    }

    _asciiString = std::make_unique<src::AsciiString>(stream);

    if (hasAdditionalSections())
    {
        // The callouts section is currently the only extra subsection type
        _callouts = std::make_unique<src::Callouts>(stream);
    }
}

void SRC::flatten(Stream& stream)
{
    stream << _header << _version << _flags << _reserved1B << _wordCount
           << _reserved2B << _size;

    for (auto& word : _hexData)
    {
        stream << word;
    }

    _asciiString->flatten(stream);

    if (_callouts)
    {
        _callouts->flatten(stream);
    }
}

SRC::SRC(Stream& pel)
{
    try
    {
        unflatten(pel);
        validate();
    }
    catch (const std::exception& e)
    {
        log<level::ERR>("Cannot unflatten SRC", entry("ERROR=%s", e.what()));
        _valid = false;
    }
}

void SRC::validate()
{
    bool failed = false;

    if ((header().id != static_cast<uint16_t>(SectionID::primarySRC)) &&
        (header().id != static_cast<uint16_t>(SectionID::secondarySRC)))
    {
        log<level::ERR>("Invalid SRC section ID",
                        entry("ID=0x%X", header().id));
        failed = true;
    }

    // Check the version in the SRC, not in the header
    if (_version != srcSectionVersion)
    {
        log<level::ERR>("Invalid SRC section version",
                        entry("VERSION=0x%X", _version));
        failed = true;
    }

    _valid = failed ? false : true;
}

} // namespace pels
} // namespace openpower
