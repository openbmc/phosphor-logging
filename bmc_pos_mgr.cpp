// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright OpenBMC Authors

#include "config.h"

#include "bmc_pos_mgr.hpp"

#include "log_manager.hpp"
#include "util.hpp"

#include <phosphor-logging/lg2.hpp>
#include <xyz/openbmc_project/Inventory/Decorator/Position/client.hpp>

namespace phosphor::logging
{

constexpr uint32_t noPosition = 0xFF;
constexpr uint32_t entryIdValueMask = 0x00FFFFFF;

void BMCPosMgr::init(sdbusplus::bus_t& bus)
{
    setBMCPosition(getBMCPosition(bus));
}

void BMCPosMgr::setBMCPosition(const std::optional<size_t>& bmcPosFromDBus)
{
    if (!bmcPosFromDBus.has_value())
    {
        // Not on D-Bus
        lg2::error("Could not get BMC position");
        bmcPosition = std::nullopt;
    }
    else if (bmcPosFromDBus.value() == std::numeric_limits<size_t>::max())
    {
        // On D-Bus, but position can't be obtained from the HW
        lg2::warning("BMC position value could not be obtained from hardware");
        bmcPosition = std::nullopt;
    }
    else
    {
        auto pos = static_cast<uint32_t>(bmcPosFromDBus.value());
        lg2::info("Found BMC position {POSITION}", "POSITION", pos);
        bmcPosition = pos;
    }
}

uint32_t BMCPosMgr::processEntryId(uint32_t id)
{
    setPrefixInEntryId(id);

    // Roll it over at 0x00FFFFFF
    checkEntryIdRollover(id);

    if (idHasNoPosition(id))
    {
        idsWithNoPosition.push_back(id + 1);
    }

    return id;
}

void BMCPosMgr::setPrefixInEntryId(uint32_t& id)
{
    uint32_t posField = bmcPosition.value_or(noPosition);
    uint32_t prefix = (posField & 0xFF) << 24;

    id &= entryIdValueMask;
    id |= prefix;
}

bool BMCPosMgr::idHasNoPosition(uint32_t id)
{
    return (id & ~entryIdValueMask) == (noPosition << 24);
}

void BMCPosMgr::checkEntryIdRollover(uint32_t& id)
{
    if ((id & entryIdValueMask) == entryIdValueMask)
    {
        lg2::info("Event log ID rolled over");
        id &= ~entryIdValueMask;
    }
}

std::optional<size_t> BMCPosMgr::getBMCPosition(sdbusplus::bus_t& bus)
{
    using Position = sdbusplus::client::xyz::openbmc_project::inventory::
        decorator::Position<>;

    try
    {
        auto service =
            util::getService(bus, BMC_POS_OBJECT_PATH, Position::interface);

        return util::getProperty<size_t>(bus, service, BMC_POS_OBJECT_PATH,
                                         Position::interface, "Position");
    }
    catch (const std::exception& e)
    {
        lg2::error("Unable to get BMC position: {ERROR}", "ERROR", e);
    }

    return std::nullopt;
}

void BMCPosMgr::removeNoPosLogs(internal::Manager& logMgr)
{
    // If the position is now known, remove any logs created
    // when there wasn't one.
    if (!idsWithNoPosition.empty() && bmcPosition.has_value())
    {
        std::ranges::for_each(idsWithNoPosition, [&logMgr](auto id) {
            lg2::info(
                "Removing log {ID} that didn't have a position because now there is one",
                "ID", id);
            logMgr.erase(id);
        });
        idsWithNoPosition.clear();
    }
}

bool BMCPosMgr::idContainsCurrentPosition(uint32_t id) const
{
    return (id >> 24) == bmcPosition.value_or(noPosition);
}

} // namespace phosphor::logging
