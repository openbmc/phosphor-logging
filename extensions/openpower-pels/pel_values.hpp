#pragma once

#include <string>
#include <tuple>
#include <vector>

namespace openpower
{
namespace pels
{
namespace pel_values
{

// The actual value as it shows up in the PEL
const int fieldValuePos = 0;

// The name of the value as specified in the message registry
const int registryNamePos = 1;

// The description of the field, used by PEL parsers
const int descriptionPos = 2;

using PELFieldValue = std::tuple<uint32_t, const char*, const char*>;
using PELValues = std::vector<PELFieldValue>;

/**
 * @brief Find the desired entry in a PELValues table based on the
 *        field value.
 *
 * @param[in] value - the PEL value to find
 * @param[in] fields - the PEL values table to use
 *
 * @return PELValues::const_iterator - an iterator to the table entry
 */
PELValues::const_iterator findByValue(uint32_t value, const PELValues& fields);

/**
 * @brief Find the desired entry in a PELValues table based on the
 *        field message registry name.
 *
 * @param[in] name - the PEL message registry enum name
 * @param[in] fields - the PEL values table to use
 *
 * @return PELValues::const_iterator - an iterator to the table entry
 */
PELValues::const_iterator findByName(const std::string& name,
                                     const PELValues& fields);

/**
 * @brief The values for the 'subsystem' field in the User Header
 */
extern const PELValues subsystemValues;

/**
 * @brief The values for the 'severity' field in the User Header
 */
extern const PELValues severityValues;

/**
 * @brief The values for the 'Event Type' field in the User Header
 */
extern const PELValues eventTypeValues;

/**
 * @brief The values for the 'Event Scope' field in the User Header
 */
extern const PELValues eventScopeValues;

/**
 * @brief The values for the 'Action Flags' field in the User Header
 */
extern const PELValues actionFlagsValues;

/**
 * @brief The values for callout priorities in the SRC section
 */
extern const PELValues calloutPriorityValues;

} // namespace pel_values
} // namespace pels
} // namespace openpower
