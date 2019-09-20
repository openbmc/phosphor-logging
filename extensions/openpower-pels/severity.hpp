#pragma once

#include "elog_entry.hpp"

namespace openpower
{
namespace pels
{

/**
 * @brief Says if the OpenBMC event log severity and the PEL severity match.
 *
 * As there are many more PEL severities, multiple PEL severities can map
 * to the same OpenBMC event log severity.
 *
 * @param[in] severity - The OpenBMC event log severity
 * @param[in] pelSeverity - The PEL severity
 *
 * @return bool - true if they match, false else
 */
bool pelSeverityMatch(phosphor::logging::Entry::Level severity,
                      uint8_t pelSeverity);

/**
 * @brief Convert an OpenBMC event log severity to a PEL severity
 *
 * @param[in] severity - The OpenBMC event log severity
 *
 * @return uint8_t - The PEL severity value
 */
uint8_t convertOBMCSeverityToPEL(phosphor::logging::Entry::Level severity);

} // namespace pels
} // namespace openpower
