#include "callouts.hpp"

namespace openpower
{
namespace pels
{
namespace src
{

Callouts::Callouts(Stream& pel)
{
    pel >> _subsectionID >> _subsectionFlags >> _subsectionWordLength;

    size_t currentLength = sizeof(_subsectionID) + sizeof(_subsectionFlags) +
                           sizeof(_subsectionWordLength);

    while ((_subsectionWordLength * 4) > currentLength)
    {
        _callouts.emplace_back(new Callout(pel));
        currentLength += _callouts.back()->flattenedSize();
    }
}

void Callouts::flatten(Stream& pel)
{
    pel << _subsectionID << _subsectionFlags << _subsectionWordLength;

    for (auto& callout : _callouts)
    {
        callout->flatten(pel);
    }
}

} // namespace src
} // namespace pels
} // namespace openpower
