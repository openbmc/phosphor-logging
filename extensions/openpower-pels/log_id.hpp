#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace openpower
{
namespace pels
{

namespace position
{
/**
 * @brief Stores the BMC position from the most recent event log.
 */
extern uint32_t bmcPosition;

/**
 * @brief Sets 'position::bmcPosition' based on the position in
 *        the upper byte of the passed in obmc ID
 *
 * @param[in] obmcID - The ID to extract the position from
 */
void extractBMCPositionFromLogID(uint32_t obmcID);

/**
 * @brief Returns the BMC position
 *
 * @return The BMC position, or std::nullopt if not available.
 */
std::optional<uint32_t> getBMCPosition();

} // namespace position

namespace detail
{

/**
 * @brief Adds the 1 byte log creator prefix to the log ID
 *
 * The 2nd nibble is the BMC position, if that feature is
 * enabled.
 *
 * @param[in] id - the ID to add it to
 *
 * @return - the full log ID
 */
uint32_t addLogIDPrefix(uint32_t id);

/**
 * @brief Generates a PEL ID based on the current time.
 *
 * Used for error scenarios where the normal method doesn't
 * work in order to get a unique ID still.
 *
 * @return A unique log ID.
 */
uint32_t getTimeBasedLogID();

} // namespace detail

/**
 * @brief Generates a unique PEL log entry ID every time
 *        it is called.
 *
 * This ID is used at offset 0x2C in the Private Header
 * section of a PEL.  For single BMC systems, it must
 * start with 0x50.
 *
 * @return uint32_t - The log ID
 */
uint32_t generatePELID();

/**
 * @brief Check for file containing zero data.
 *
 */
void checkFileForZeroData(const std::string& filename);

} // namespace pels
} // namespace openpower
