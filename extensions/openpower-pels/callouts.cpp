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
#include "callouts.hpp"

#include <phosphor-logging/log.hpp>

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

void Callouts::flatten(Stream& pel) const
{
    pel << _subsectionID << _subsectionFlags << _subsectionWordLength;

    for (auto& callout : _callouts)
    {
        callout->flatten(pel);
    }
}

void Callouts::addCallout(std::unique_ptr<Callout> callout)
{
    if (_callouts.size() < maxNumberOfCallouts)
    {
        _callouts.push_back(std::move(callout));

        _subsectionWordLength += _callouts.back()->flattenedSize() / 4;
    }
    else
    {
        using namespace phosphor::logging;
        log<level::INFO>("Dropping PEL callout because at max");
    }

    // Mapping including the  3 Medium levels as A,B and C
    const std::map<std::uint8_t, int> priorities = {
        {'H', 10}, {'M', 9}, {'A', 8}, {'B', 7}, {'C', 6}, {'L', 5}};

    auto sortPriority = [&priorities](const std::unique_ptr<Callout>& p1,
                                      const std::unique_ptr<Callout>& p2) {
        return priorities.at(p1->priority()) > priorities.at(p2->priority());
    };

    std::sort(_callouts.begin(), _callouts.end(), sortPriority);
}

} // namespace src
} // namespace pels
} // namespace openpower