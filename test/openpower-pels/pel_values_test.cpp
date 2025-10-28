// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "extensions/openpower-pels/pel_values.hpp"

#include <gtest/gtest.h>

using namespace openpower::pels;
using namespace openpower::pels::pel_values;

TEST(PELFieldsTest, TestFindFields)
{
    auto s = findByValue(0x5D, subsystemValues);
    ASSERT_NE(s, subsystemValues.end());
    EXPECT_EQ(0x5D, std::get<fieldValuePos>(*s));
    EXPECT_STREQ("cec_service_network", std::get<registryNamePos>(*s));

    s = findByName("cec_clocks", subsystemValues);
    ASSERT_NE(s, subsystemValues.end());
    EXPECT_EQ(0x58, std::get<fieldValuePos>(*s));
    EXPECT_STREQ("cec_clocks", std::get<registryNamePos>(*s));
    EXPECT_STREQ("CEC Hardware - Clock", std::get<descriptionPos>(*s));

    s = findByValue(0xFF, subsystemValues);
    EXPECT_EQ(s, subsystemValues.end());

    s = findByName("foo", subsystemValues);
    EXPECT_EQ(s, subsystemValues.end());
}
