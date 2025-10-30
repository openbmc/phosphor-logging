// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright OpenBMC Authors

#include "bmc_pos_mgr.hpp"

#include <phosphor-logging/lg2.hpp>

#include <fstream>

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

void BMCPosMgr::readBMCPosition()
{
    bmcPosition = noPosition;

    std::error_code ec;
    if (!std::filesystem::exists(posFile, ec))
    {
        lg2::error("BMC position file {FILE} doesn't exist", "FILE", posFile);
        return;
    }

    std::ifstream stream{posFile};
    if (!stream)
    {
        lg2::error("Could not open BMC position file {FILE}", "FILE", posFile);
        return;
    }

    size_t position;
    stream >> position;

    if (stream.fail())
    {
        lg2::error("Failed reading BMC position out of {FILE}", "FILE",
                   posFile);
        return;
    }

    if (position == std::numeric_limits<size_t>::max())
    {
        lg2::error("BMC position value could not be obtained from hardware");
        return;
    }

    bmcPosition = static_cast<uint8_t>(position);
    lg2::info("Found BMC position {POSITION}", "POSITION", bmcPosition);
}

bool BMCPosMgr::idContainsCurrentPosition(uint32_t id) const
{
    return (id >> 24) == bmcPosition;
}

} // namespace phosphor::logging
