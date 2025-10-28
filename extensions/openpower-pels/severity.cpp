// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "severity.hpp"

namespace openpower
{
namespace pels
{

using LogSeverity = phosphor::logging::Entry::Level;

namespace
{

LogSeverity convertPELSeverityToOBMC(SeverityType pelSeverity)
{
    LogSeverity logSeverity = LogSeverity::Error;

    const std::map<SeverityType, LogSeverity> severities{
        {SeverityType::nonError, LogSeverity::Informational},
        {SeverityType::recovered, LogSeverity::Informational},
        {SeverityType::predictive, LogSeverity::Warning},
        {SeverityType::unrecoverable, LogSeverity::Error},
        {SeverityType::critical, LogSeverity::Critical},
        {SeverityType::diagnostic, LogSeverity::Error},
        {SeverityType::symptom, LogSeverity::Warning}};

    auto s = severities.find(pelSeverity);
    if (s != severities.end())
    {
        logSeverity = s->second;
    }

    return logSeverity;
}

} // namespace

uint8_t convertOBMCSeverityToPEL(LogSeverity severity)
{
    uint8_t pelSeverity = static_cast<uint8_t>(SeverityType::unrecoverable);
    switch (severity)
    {
        case (LogSeverity::Notice):
        case (LogSeverity::Informational):
        case (LogSeverity::Debug):
            pelSeverity = static_cast<uint8_t>(SeverityType::nonError);
            break;

        case (LogSeverity::Warning):
            pelSeverity = static_cast<uint8_t>(SeverityType::predictive);
            break;

        case (LogSeverity::Critical):
            pelSeverity = static_cast<uint8_t>(SeverityType::critical);
            break;

        case (LogSeverity::Emergency):
        case (LogSeverity::Alert):
        case (LogSeverity::Error):
            pelSeverity = static_cast<uint8_t>(SeverityType::unrecoverable);
            break;
    }

    return pelSeverity;
}

std::optional<LogSeverity> fixupLogSeverity(LogSeverity obmcSeverity,
                                            SeverityType pelSeverity)
{
    bool isNonErrPelSev = (pelSeverity == SeverityType::nonError) ||
                          (pelSeverity == SeverityType::recovered);

    bool isNonErrObmcSev = (obmcSeverity == LogSeverity::Notice) ||
                           (obmcSeverity == LogSeverity::Informational) ||
                           (obmcSeverity == LogSeverity::Debug);

    // If a nonError/recovered PEL, then the LogSeverity must be
    // Notice/Informational/Debug, otherwise set it to Informational.
    if (isNonErrPelSev && !isNonErrObmcSev)
    {
        return LogSeverity::Informational;
    }

    // If a Notice/Informational/Debug LogSeverity, then the PEL
    // severity must be nonError/recovered, otherwise set it
    // to an appropriate value based on the actual PEL severity.
    if (isNonErrObmcSev && !isNonErrPelSev)
    {
        return convertPELSeverityToOBMC(pelSeverity);
    }

    // If PEL is critical, the LogSeverity should be as well.
    if ((obmcSeverity != LogSeverity::Critical) &&
        (pelSeverity == SeverityType::critical))
    {
        return LogSeverity::Critical;
    }

    return std::nullopt;
}

} // namespace pels
} // namespace openpower
