// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2020 IBM Corporation

#include "service_indicators.hpp"

#include <phosphor-logging/lg2.hpp>

#include <bitset>

namespace openpower::pels::service_indicators
{

static constexpr auto platformSaiLedGroup =
    "/xyz/openbmc_project/led/groups/platform_system_attention_indicator";

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
            // Find the inventory paths for those location codes.
            auto paths = getInventoryPaths(locCodes);
            if (!paths.empty())
            {
                setNotFunctional(paths);
                createCriticalAssociation(paths);
                sai = false;
            }
        }
    }

    if (sai)
    {
        try
        {
            _dataIface.assertLEDGroup(platformSaiLedGroup, true);
        }
        catch (const std::exception& e)
        {
            lg2::error("Failed to assert platform SAI LED group: {EXCEPTION}",
                       "EXCEPTION", e);
        }
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

std::vector<std::string> LightPath::getInventoryPaths(
    const std::vector<std::string>& locationCodes) const
{
    std::vector<std::string> paths;

    for (const auto& locCode : locationCodes)
    {
        try
        {
            auto inventoryPaths =
                _dataIface.getInventoryFromLocCode(locCode, 0, true);
            for (const auto& path : inventoryPaths)
            {
                if (std::find(paths.begin(), paths.end(), path) == paths.end())
                {
                    paths.push_back(path);
                }
            }
        }
        catch (const std::exception& e)
        {
            lg2::error("Could not get inventory path for "
                       "location code {LOCCODE} ({EXCEPTION}).",
                       "LOCCODE", locCode, "EXCEPTION", e);

            // Unless we can set the LEDs for all FRUs, we can't turn
            // on any of them, so clear the list and quit.
            paths.clear();
            break;
        }
    }

    return paths;
}

void LightPath::setNotFunctional(
    const std::vector<std::string>& inventoryPaths) const
{
    for (const auto& path : inventoryPaths)
    {
        try
        {
            _dataIface.setFunctional(path, false);
        }
        catch (const std::exception& e)
        {
            lg2::info(
                "Could not write Functional property on {PATH} ({EXCEPTION})",
                "PATH", path, "EXCEPTION", e);
        }
    }
}

void LightPath::createCriticalAssociation(
    const std::vector<std::string>& inventoryPaths) const
{
    for (const auto& path : inventoryPaths)
    {
        try
        {
            _dataIface.setCriticalAssociation(path);
        }
        catch (const std::exception& e)
        {
            lg2::info(
                "Could not set critical association on object path {PATH} ({EXCEPTION})",
                "PATH", path, "EXCEPTION", e);
        }
    }
}

} // namespace openpower::pels::service_indicators
