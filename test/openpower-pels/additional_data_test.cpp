// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "extensions/openpower-pels/additional_data.hpp"

#include <gtest/gtest.h>

using namespace openpower::pels;

TEST(AdditionalDataTest, GetKeywords)
{
    std::map<std::string, std::string> data{
        {"KEY1", "VALUE1"}, {"KEY2", "VALUE2"}, {"KEY3", ""}};
    AdditionalData ad{data};

    EXPECT_TRUE(ad.getValue("KEY1"));
    EXPECT_EQ(*(ad.getValue("KEY1")), "VALUE1");

    EXPECT_TRUE(ad.getValue("KEY2"));
    EXPECT_EQ(*(ad.getValue("KEY2")), "VALUE2");

    EXPECT_FALSE(ad.getValue("x"));

    auto value3 = ad.getValue("KEY3");
    EXPECT_TRUE(value3);
    EXPECT_TRUE((*value3).empty());

    EXPECT_FALSE(ad.getValue("HELLOWORLD"));
    EXPECT_FALSE(ad.getValue("VALUE5"));

    auto json = ad.toJSON();
    std::string expected = R"({"KEY1":"VALUE1","KEY2":"VALUE2","KEY3":""})";
    EXPECT_EQ(json.dump(), expected);

    ad.remove("KEY1");
    EXPECT_FALSE(ad.getValue("KEY1"));
}

TEST(AdditionalDataTest, AddData)
{
    AdditionalData ad;

    ad.add("KEY1", "VALUE1");
    EXPECT_EQ(*(ad.getValue("KEY1")), "VALUE1");

    ad.add("KEY2", "VALUE2");
    EXPECT_EQ(*(ad.getValue("KEY2")), "VALUE2");

    std::map<std::string, std::string> expected{{"KEY1", "VALUE1"},
                                                {"KEY2", "VALUE2"}};

    EXPECT_EQ(expected, ad.getData());
}
