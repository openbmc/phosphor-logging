// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright OpenBMC Authors
#pragma once

#include <cstdint>
#include <filesystem>

namespace phosphor::logging
{

const std::filesystem::path bmcPositionFile = "/run/openbmc/bmc_position";

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
    ~BMCPosMgr() = default;
    BMCPosMgr(const BMCPosMgr&) = delete;
    BMCPosMgr& operator=(const BMCPosMgr&) = delete;
    BMCPosMgr(BMCPosMgr&&) = delete;
    BMCPosMgr& operator=(BMCPosMgr&&) = delete;

    explicit BMCPosMgr(const std::filesystem::path& posFile = bmcPositionFile) :
        posFile(posFile)
    {
        readBMCPosition();
    }

    /**
     * @brief Updates entryID with the current BMC position value
     *        and returns it.
     *
     * Rolls over the ID if necessary.
     *
     * @param[in] id - The entry ID
     *
     * @return The updated entry ID
     */
    uint32_t processEntryId(uint32_t id) const;

    /**
     * @brief Reads the BMC position property from the filesystem
     *        sets bmcPosition appropriately
     */
    void readBMCPosition();

    /**
     * @brief Says if the current position contains a
     *        valid position value.
     */
    bool isPositionValid() const;

    /**
     * @brief Returns the BMC position
     */
    uint8_t getBMCPosition() const
    {
        return bmcPosition;
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
    void setPrefixInEntryId(uint32_t& id) const;

    /**
     * @brief If the ID passed in is the last one before a
     *        rollover, do the rollover.
     *
     *        e.g. 0x00FFFFFF will roll over to 0x00000000
     *
     * @param[inout] The ID that may be rolled over
     */
    void checkEntryIdRollover(uint32_t& id) const;

    /**
     * @brief The BMC position.
     *
     * 0xFF if the position can't be obtained.
     */
    uint8_t bmcPosition{};

    /**
     * @brief The BMC position file
     */
    std::filesystem::path posFile;
};

} // namespace phosphor::logging
