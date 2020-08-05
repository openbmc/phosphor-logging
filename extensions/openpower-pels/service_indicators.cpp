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
    auto creator = pel.privateHeader().creatorID();

    // Don't ignore serviceable BMC or hostboot errors
    if ((static_cast<CreatorID>(creator) == CreatorID::openBMC) ||
        (static_cast<CreatorID>(creator) == CreatorID::hostboot))
    {
        std::bitset<16> actionFlags{pel.userHeader().actionFlags()};
        if (actionFlags.test(serviceActionFlagBit))
        {
            return false;
        }
    }

    return true;
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
    std::vector<std::string> locCodes;
    bool firstCallout = true;
    uint8_t firstCalloutPriority;

    // Collect location codes for the first group of callouts,
    // where a group can be:
    //  * a single medium priority callout
    //  * one or more high priority callouts
    //  * one or more medium group a priority callouts
    //
    // All callouts in the group must be hardware callouts.

    for (const auto& callout : callouts)
    {
        if (firstCallout)
        {
            firstCallout = false;

            firstCalloutPriority = callout->priority();

            // If the first callout is High, Medium, or Medium
            // group A, and is a hardware callout, then we
            // want it.
            if (isRequiredPriority(firstCalloutPriority) &&
                isHardwareCallout(*callout))
            {
                locCodes.push_back(callout->locationCode());
            }
            else
            {
                break;
            }

            // By definition a medium priority callout can't be part
            // of a group, so no need to look for more.
            if (static_cast<CalloutPriority>(firstCalloutPriority) ==
                CalloutPriority::medium)
            {
                break;
            }
        }
        else
        {
            // Only continue while the callouts are the same
            // priority as the first callout.
            if (callout->priority() != firstCalloutPriority)
            {
                break;
            }

            // If any callout in the group isn't a hardware callout,
            // then don't light up any LEDs at all.
            if (!isHardwareCallout(*callout))
            {
                locCodes.clear();
                break;
            }

            locCodes.push_back(callout->locationCode());
        }
    }

    return locCodes;
}

bool LightPath::isRequiredPriority(uint8_t priority) const
{
    auto calloutPriority = static_cast<CalloutPriority>(priority);
    return (calloutPriority == CalloutPriority::high) ||
           (calloutPriority == CalloutPriority::medium) ||
           (calloutPriority == CalloutPriority::mediumGroupA);
}

bool LightPath::isHardwareCallout(const src::Callout& callout) const
{
    const auto& fruIdentity = callout.fruIdentity();
    if (fruIdentity)
    {
        return (callout.locationCodeSize() != 0) &&
               ((fruIdentity->failingComponentType() ==
                 src::FRUIdentity::hardwareFRU) ||
                (fruIdentity->failingComponentType() ==
                 src::FRUIdentity::symbolicFRUTrustedLocCode));
    }

    return false;
}

std::vector<std::string> LightPath::getLEDGroupPaths(
    const std::vector<std::string>& locationCodes) const
{
    std::vector<std::string> ledGroups;
    std::string inventoryPath;

    for (const auto& locCode : locationCodes)
    {
        try
        {
            inventoryPath =
                _dataIface.getInventoryFromLocCode(locCode, 0, true);
        }
        catch (const std::exception& e)
        {
            log<level::ERR>(fmt::format("Could not get inventory path for "
                                        "location code {} ({}).",
                                        locCode, e.what())
                                .c_str());

            // Unless we can get the LEDs for all FRUs, we can't turn
            // on any of them, so clear the list and quit.
            ledGroups.clear();
            break;
        }

        try
        {
            ledGroups.push_back(_dataIface.getFaultLEDGroup(inventoryPath));
        }
        catch (const std::exception& e)
        {
            log<level::ERR>(fmt::format("Could not get LED group path for "
                                        "inventory path {} ({}).",
                                        inventoryPath, e.what())
                                .c_str());
            ledGroups.clear();
            break;
        }
    }

    return ledGroups;
}

void LightPath::assertLEDs(const std::vector<std::string>& ledGroups) const
{
    for (const auto& ledGroup : ledGroups)
    {
        try
        {
            _dataIface.assertLEDGroup(ledGroup, true);
        }
        catch (const std::exception& e)
        {
            log<level::ERR>(fmt::format("Failed to assert LED group {} ({})",
                                        ledGroup, e.what())
                                .c_str());
        }
    }
}

} // namespace openpower::pels::service_indicators
