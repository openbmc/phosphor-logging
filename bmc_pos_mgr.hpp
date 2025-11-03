// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright OpenBMC Authors
#pragma once

#include <sdbusplus/bus.hpp>

#include <cstdint>
#include <optional>

namespace phosphor::logging
{

namespace internal
{
class Manager;
};

/**
 * @class BMCPosMgr
 *
 * Manages encoding the BMC position into the upper byte
 * of the event log ID.  If the position isn't there, it will
 * use 0xFF instead.
 *
 */
class BMCPosMgr
{
  public:
    BMCPosMgr() = default;
    ~BMCPosMgr() = default;
    BMCPosMgr(const BMCPosMgr&) = delete;
    BMCPosMgr& operator=(const BMCPosMgr&) = delete;
    BMCPosMgr(BMCPosMgr&&) = delete;
    BMCPosMgr& operator=(BMCPosMgr&&) = delete;

    /**
     * @brief The BMC position.  std::nullopt if it isn't on
     *        D-Bus or is the magic value that means it isn't
     *        possible to ascertain for some reason.
     */
    std::optional<uint32_t> bmcPosition;

    /**
     * @brief Gets the BMC position from D-Bus
     *
     * @param[in] bus - the sdbusplus bus
     */
    void init(sdbusplus::bus_t& bus);

    /**
     * @brief Sets the bmcPosition member var from
     *        the D-Bus property value.
     *
     * A D-Bus value of std::numeric_limits<size_t>::max() translates
     * to a bmcPosition value of std::nullopt;
     *
     * @param[in] bmcPosFromDBus - The D-Bus property value
     */
    void setBMCPosition(const std::optional<size_t>& bmcPosFromDBus);

    /**
     * @brief Says if the ID passed in has the 'no position' value
     *        encoded into it.
     *
     * @param[in] id - The entry ID
     *
     * @return true - If the ID contains 'no position'
     */
    bool idHasNoPosition(uint32_t id);

    /**
     * @brief Updates entryID with the current BMC position value
     *        and returns it.
     *
     * Adds the ID to idsWithNoPosition if there is no position.
     * Rolls over the ID if necessary.
     *
     * @param[in] id - The entry ID
     *
     * @return The updated entry ID
     */
    uint32_t processEntryId(uint32_t id);

    /**
     * @brief Deletes all event logs that have IDs in idsWithNoPosition
     *        if there is now a position.
     *
     * @param[in] logMgr - The Manager object that can erase event logs
     */
    void removeNoPosLogs(internal::Manager& logMgr);

    /**
     * @brief Adds an entry ID with no position to the list
     *
     * @param[in] id - The ID to add
     */
    inline void addNoPosEntryId(uint32_t id)
    {
        idsWithNoPosition.push_back(id);
    }

    /**
     * @brief Returns true if the position encoded in the
     *        ID passed in contains the current BMC position.
     *
     * @param[in] id - The entry ID to check
     */
    bool idContainsCurrentPosition(uint32_t id) const;

  private:
    /**
     * @brief Sets the position prefix in the entry ID
     *
     * @param[inout] id - The entry ID
     */
    void setPrefixInEntryId(uint32_t& id);

    /**
     * @brief Reads the BMC position property from D-Bus
     *
     * @param[in] bus - the sdbusplus bus
     * @return The position property value. std::nullopt if not on D-Bus
     */
    std::optional<size_t> getBMCPosition(sdbusplus::bus_t& bus);

    /**
     * @brief If the ID passed in is the last one before a
     *        rollover, do the rollover.
     *
     *        e.g. 0x00FFFFFF will roll over to 0x00000000
     *
     * @param[inout] The ID that may be rolled over
     */
    void checkEntryIdRollover(uint32_t& id);

    /**
     * @brief All event log IDs that have 'no position' in them.
     */
    std::vector<uint32_t> idsWithNoPosition;
};

} // namespace phosphor::logging
