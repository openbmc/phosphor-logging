// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "extensions/openpower-pels/json_utils.hpp"
#include "extensions/openpower-pels/paths.hpp"

#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

using namespace openpower::pels;

TEST(JsonUtilsTest, TrimEndTest)
{
    std::string testStr("Test string 1");
    EXPECT_EQ(trimEnd(testStr), "Test string 1");
    testStr = "Test string 2 ";
    EXPECT_EQ(trimEnd(testStr), "Test string 2");
    testStr = " Test string 3  ";
    EXPECT_EQ(trimEnd(testStr), " Test string 3");
}

TEST(JsonUtilsTest, NumberToStringTest)
{
    size_t number = 123;
    EXPECT_EQ(getNumberString("%d", number), "123");
    EXPECT_EQ(getNumberString("%03X", number), "07B");
    EXPECT_EQ(getNumberString("0x%X", number), "0x7B");
    EXPECT_THROW(getNumberString("%123", number), std::invalid_argument);
}

TEST(JsonUtilsTest, JsonInsertTest)
{
    std::string json;
    jsonInsert(json, "Key", "Value1", 1);
    EXPECT_EQ(json, "    \"Key\":                      \"Value1\",\n");
    jsonInsert(json, "Keyxxxxxxxxxxxxxxxxxxxxxxxxxx", "Value2", 2);
    EXPECT_EQ(json, "    \"Key\":                      \"Value1\",\n"
                    "        \"Keyxxxxxxxxxxxxxxxxxxxxxxxxxx\": \"Value2\",\n");
}

TEST(JsonUtilsTest, GetComponentNameTest)
{
    const auto compIDs = R"(
    {
        "1000": "some comp",
        "2222": "another comp"
    })"_json;

    auto dataPath = getPELReadOnlyDataPath();
    std::ofstream file{dataPath / "O_component_ids.json"};
    file << compIDs;
    file.close();

    // The component ID file exists
    EXPECT_EQ(getComponentName(0x1000, 'O'), "some comp");
    EXPECT_EQ(getComponentName(0x2222, 'O'), "another comp");
    EXPECT_EQ(getComponentName(0x0001, 'O'), "0x0001");

    // No component ID file
    EXPECT_EQ(getComponentName(0x3456, 'B'), "0x3456");

    // PHYP, uses characters if both bytes nonzero
    EXPECT_EQ(getComponentName(0x4552, 'H'), "ER");
    EXPECT_EQ(getComponentName(0x584D, 'H'), "XM");
    EXPECT_EQ(getComponentName(0x5800, 'H'), "0x5800");
    EXPECT_EQ(getComponentName(0x0058, 'H'), "0x0058");

    std::filesystem::remove_all(dataPath);
}
