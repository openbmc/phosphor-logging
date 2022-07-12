#pragma once

#include "elog_entry.hpp"
#include "pel_types.hpp"

#include <optional>

namespace openpower
{
namespace pels
{

/**
 * @brief Convert an OpenBMC event log severity to a PEL severity
 *
 * @param[in] severity - The OpenBMC event log severity
 *
 * @return uint8_t - The PEL severity value
 */
uint8_t convertOBMCSeverityToPEL(phosphor::logging::Entry::Level severity);

/**
 * @brief Possibly calculate a new LogSeverity value based on the
 *        current LogSeverity and PEL severity.
 *
 * Just handles cases where the LogSeverity value is clearly out of
 * sync with the PEL severity:
 * - critical PEL but non critical LogSeverity
 * - info PEL but non info LogSeverity
 * - non info PEL but info LogSeverity
 *
 * @param[in] obmcSeverity - The current LogSeverity
 * @param[in] pelSeverity - The PEL severity
 *
 * @return optional<LogSeverity> The new LogSeverity to use if one was
 *         found, otherwise std::nullopt which means the original one
 *         is good enough.
 */
std::optional<phosphor::logging::Entry::Level>
    fixupLogSeverity(phosphor::logging::Entry::Level obmcSeverity,
                     SeverityType pelSeverity);
} // namespace pels
} // namespace openpower
