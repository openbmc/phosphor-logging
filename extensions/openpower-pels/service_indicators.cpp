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
#include "service_indicators.hpp"

#include <fmt/format.h>

#include <bitset>
#include <phosphor-logging/log.hpp>

namespace openpower::pels::service_indicators
{

using namespace phosphor::logging;

std::unique_ptr<Policy> getPolicy(const DataInterfaceBase& dataIface)
{
    // At the moment there is just one type of policy.
    return std::make_unique<LightPath>(dataIface);
}

bool LightPath::ignore(const PEL& pel) const
{
    // TODO
    return false;
}

void LightPath::activate(const PEL& pel)
{
    if (ignore(pel))
    {
        return;
    }

    // Now that we've gotten this far, we'll need to turn on
    // the system attention indicator if we don't find other
    // indicators to turn on.
    bool sai = true;
    auto src = pel.primarySRC();
    const auto& calloutsObj = (*src)->callouts();

    if (calloutsObj && !calloutsObj->callouts().empty())
    {
        const auto& callouts = calloutsObj->callouts();

        // From the callouts, find the location codes whose
        // LEDs need to be turned on.
        auto locCodes = getLocationCodes(callouts);
        if (!locCodes.empty())
        {
            // Find the LED groups for those location codes.
            auto ledPaths = getLEDGroupPaths(locCodes);
            if (!ledPaths.empty())
            {
                // Tell the LED groups to assert their LEDs.
                assertLEDs(ledPaths);
                sai = false;
            }
        }
    }

    if (sai)
    {
        log<level::INFO>("The System Attention Indicator needs to be turned "
                         "on, when available");
    }
}

std::vector<std::string> LightPath::getLocationCodes(
    const std::vector<std::unique_ptr<src::Callout>>& callouts) const
{
    // TODO
    return {};
}

bool LightPath::isRequiredPriority(uint8_t priority) const
{
    // TODO
    return true;
}

bool LightPath::isHardwareCallout(const src::Callout& callout) const
{
    // TODO
    return false;
}

std::vector<std::string> LightPath::getLEDGroupPaths(
    const std::vector<std::string>& locationCodes) const
{
    // TODO
    return {};
}

void LightPath::assertLEDs(const std::vector<std::string>& ledGroups) const
{
    // TODO
}

} // namespace openpower::pels::service_indicators
