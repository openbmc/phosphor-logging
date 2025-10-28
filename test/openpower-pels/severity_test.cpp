// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "extensions/openpower-pels/severity.hpp"

#include <gtest/gtest.h>

using namespace openpower::pels;
using LogSeverity = phosphor::logging::Entry::Level;

TEST(SeverityTest, SeverityMapTest)
{
    ASSERT_EQ(convertOBMCSeverityToPEL(LogSeverity::Informational), 0x00);
    ASSERT_EQ(convertOBMCSeverityToPEL(LogSeverity::Notice), 0x00);
    ASSERT_EQ(convertOBMCSeverityToPEL(LogSeverity::Debug), 0x00);
    ASSERT_EQ(convertOBMCSeverityToPEL(LogSeverity::Warning), 0x20);
    ASSERT_EQ(convertOBMCSeverityToPEL(LogSeverity::Critical), 0x50);
    ASSERT_EQ(convertOBMCSeverityToPEL(LogSeverity::Emergency), 0x40);
    ASSERT_EQ(convertOBMCSeverityToPEL(LogSeverity::Alert), 0x40);
    ASSERT_EQ(convertOBMCSeverityToPEL(LogSeverity::Error), 0x40);
}

TEST(SeverityTest, fixupLogSeverityTest)
{
    struct TestParams
    {
        LogSeverity sevIn;
        SeverityType pelSevIn;
        std::optional<LogSeverity> sevOut;
    };

    const std::vector<TestParams> testParams{
        // Convert nonInfo sevs to info
        {LogSeverity::Error, SeverityType::nonError,
         LogSeverity::Informational},
        {LogSeverity::Critical, SeverityType::recovered,
         LogSeverity::Informational},
        {LogSeverity::Warning, SeverityType::nonError,
         LogSeverity::Informational},

        // Convert info sevs to nonInfo
        {LogSeverity::Informational, SeverityType::predictive,
         LogSeverity::Warning},
        {LogSeverity::Notice, SeverityType::unrecoverable, LogSeverity::Error},
        {LogSeverity::Debug, SeverityType::critical, LogSeverity::Critical},

        // Convert non-critical to critical
        {LogSeverity::Warning, SeverityType::critical, LogSeverity::Critical},

        // No change
        {LogSeverity::Informational, SeverityType::nonError, std::nullopt},
        {LogSeverity::Debug, SeverityType::recovered, std::nullopt},
        {LogSeverity::Notice, SeverityType::nonError, std::nullopt},
        {LogSeverity::Error, SeverityType::unrecoverable, std::nullopt},
        {LogSeverity::Critical, SeverityType::unrecoverable, std::nullopt}};

    for (const auto& test : testParams)
    {
        EXPECT_EQ(fixupLogSeverity(test.sevIn, test.pelSevIn), test.sevOut);
    }
}
