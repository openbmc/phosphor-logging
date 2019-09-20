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
