// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright OpenBMC Authors

#include "config.h"

#include "bmc_pos_mgr.hpp"

#include "util.hpp"

#include <phosphor-logging/lg2.hpp>
#include <xyz/openbmc_project/Inventory/Decorator/Position/client.hpp>

namespace phosphor::logging
{

constexpr uint8_t noPosition = 0xFF;
constexpr uint32_t entryIdValueMask = 0x00FFFFFF;

uint32_t BMCPosMgr::processEntryId(uint32_t id) const
{
    setPrefixInEntryId(id);

    // Roll it over at 0x00FFFFFF
    checkEntryIdRollover(id);

    return id;
}

bool BMCPosMgr::isPositionValid() const
{
    return bmcPosition != noPosition;
}

void BMCPosMgr::setPrefixInEntryId(uint32_t& id) const
{
    uint32_t prefix = static_cast<uint32_t>(bmcPosition) << 24;

    id &= entryIdValueMask;
    id |= prefix;
}

void BMCPosMgr::checkEntryIdRollover(uint32_t& id) const
{
    if ((id & entryIdValueMask) == entryIdValueMask)
    {
        lg2::info("Event log ID rolled over");
        id &= ~entryIdValueMask;
    }
}

void BMCPosMgr::setBMCPosition(sdbusplus::bus_t& bus)
{
    using Position = sdbusplus::client::xyz::openbmc_project::inventory::
        decorator::Position<>;

    if (IS_UNIT_TEST)
    {
        bmcPosition = noPosition;
        return;
    }

    try
    {
        auto service =
            util::getService(bus, BMC_POS_OBJECT_PATH, Position::interface);

        auto pos = util::getProperty<size_t>(bus, service, BMC_POS_OBJECT_PATH,
                                             Position::interface, "Position");

        if (pos == std::numeric_limits<size_t>::max())
        {
            // On D-Bus, but position can't be obtained from the HW
            lg2::warning(
                "BMC position value could not be obtained from hardware");
            bmcPosition = noPosition;
        }
        else
        {
            lg2::info("Found BMC position {POSITION}", "POSITION", pos);
            bmcPosition = static_cast<uint8_t>(pos);
        }
    }
    catch (const std::exception& e)
    {
        lg2::error(
            "Unable to get BMC position on {PATH} interface {IFACE}: {ERROR}",
            "PATH", BMC_POS_OBJECT_PATH, "IFACE", Position::interface, "ERROR",
            e);
        bmcPosition = noPosition;
    }
}

} // namespace phosphor::logging
