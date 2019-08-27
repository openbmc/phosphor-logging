#include "extensions/openpower-pels/pel_values.hpp"

#include <gtest/gtest.h>

using namespace openpower::pels::pel_values;

TEST(PELFieldsTest, TestFindFields)
{
    auto s = findByValue(0x5D, subsystemValues);
    ASSERT_NE(s, subsystemValues.end());
    ASSERT_EQ(0x5D, std::get<fieldValuePos>(*s));
    ASSERT_EQ("cec_service_network", std::get<registryNamePos>(*s));

    s = findByName("cec_clocks", subsystemValues);
    ASSERT_NE(s, subsystemValues.end());
    ASSERT_EQ(0x58, std::get<fieldValuePos>(*s));
    ASSERT_EQ("cec_clocks", std::get<registryNamePos>(*s));

    s = findByValue(0xFF, subsystemValues);
    ASSERT_EQ(s, subsystemValues.end());

    s = findByName("foo", subsystemValues);
    ASSERT_EQ(s, subsystemValues.end());
}
