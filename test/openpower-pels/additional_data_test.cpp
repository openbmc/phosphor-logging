#include "extensions/openpower-pels/additional_data.hpp"

#include <gtest/gtest.h>

using namespace openpower::pels;

TEST(AdditionalTest, GetKeywords)
{
    std::vector<std::string> data{"KEY1=VALUE1", "KEY2=VALUE2",
                                  "KEY3=", "HELLOWORLD", "=VALUE5"};
    AdditionalData ad{data};

    EXPECT_TRUE(ad.getValue("KEY1"));
    EXPECT_EQ(*(ad.getValue("KEY1")), "VALUE1");

    EXPECT_TRUE(ad.getValue("KEY2"));
    EXPECT_EQ(*(ad.getValue("KEY2")), "VALUE2");

    EXPECT_FALSE(ad.getValue("x"));

    auto key3 = ad.getValue("KEY3");
    EXPECT_TRUE(key3);
    EXPECT_TRUE((*key3).empty());

    EXPECT_FALSE(ad.getValue("HELLOWORLD"));
}
