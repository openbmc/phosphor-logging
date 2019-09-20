#include "extensions/openpower-pels/severity.hpp"

#include <gtest/gtest.h>

using namespace openpower::pels;
using LogSeverity = phosphor::logging::Entry::Level;

TEST(SeverityTest, PELSeverityMatchTest)
{
    ASSERT_TRUE(pelSeverityMatch(LogSeverity::Informational, 0x00));
    ASSERT_FALSE(pelSeverityMatch(LogSeverity::Informational, 0x40));

    ASSERT_TRUE(pelSeverityMatch(LogSeverity::Notice, 0x00));
    ASSERT_FALSE(pelSeverityMatch(LogSeverity::Notice, 0x40));

    ASSERT_TRUE(pelSeverityMatch(LogSeverity::Debug, 0x00));
    ASSERT_FALSE(pelSeverityMatch(LogSeverity::Debug, 0x40));

    ASSERT_TRUE(pelSeverityMatch(LogSeverity::Warning, 0x11));
    ASSERT_TRUE(pelSeverityMatch(LogSeverity::Warning, 0x25));
    ASSERT_TRUE(pelSeverityMatch(LogSeverity::Warning, 0x77));
    ASSERT_FALSE(pelSeverityMatch(LogSeverity::Warning, 0x43));

    ASSERT_TRUE(pelSeverityMatch(LogSeverity::Critical, 0x55));
    ASSERT_FALSE(pelSeverityMatch(LogSeverity::Critical, 0x40));

    ASSERT_TRUE(pelSeverityMatch(LogSeverity::Alert, 0x44));
    ASSERT_TRUE(pelSeverityMatch(LogSeverity::Alert, 0x61));
    ASSERT_FALSE(pelSeverityMatch(LogSeverity::Alert, 0x04));

    ASSERT_TRUE(pelSeverityMatch(LogSeverity::Error, 0x44));
    ASSERT_TRUE(pelSeverityMatch(LogSeverity::Error, 0x61));
    ASSERT_FALSE(pelSeverityMatch(LogSeverity::Error, 0x04));

    ASSERT_TRUE(pelSeverityMatch(LogSeverity::Emergency, 0x44));
    ASSERT_TRUE(pelSeverityMatch(LogSeverity::Emergency, 0x61));
    ASSERT_FALSE(pelSeverityMatch(LogSeverity::Emergency, 0x04));
}

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
