/**
 * Copyright © 2019 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "elog_entry.hpp"
#include "extensions/openpower-pels/generic.hpp"
#include "extensions/openpower-pels/pel.hpp"
#include "mocks.hpp"
#include "pel_utils.hpp"

#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

namespace fs = std::filesystem;
using namespace openpower::pels;
using ::testing::Return;

class PELTest : public CleanLogID
{
};

TEST_F(PELTest, FlattenTest)
{
    auto data = pelDataFactory(TestPELType::pelSimple);
    auto pel = std::make_unique<PEL>(data);

    // Check a few fields
    EXPECT_TRUE(pel->valid());
    EXPECT_EQ(pel->id(), 0x80818283);
    EXPECT_EQ(pel->plid(), 0x50515253);
    EXPECT_EQ(pel->userHeader().subsystem(), 0x10);
    EXPECT_EQ(pel->userHeader().actionFlags(), 0x80C0);

    // Test that data in == data out
    auto flattenedData = pel->data();
    EXPECT_EQ(data, flattenedData);
    EXPECT_EQ(flattenedData.size(), pel->size());
}

TEST_F(PELTest, CommitTimeTest)
{
    auto data = pelDataFactory(TestPELType::pelSimple);
    auto pel = std::make_unique<PEL>(data);

    auto origTime = pel->commitTime();
    pel->setCommitTime();
    auto newTime = pel->commitTime();

    EXPECT_NE(origTime, newTime);

    // Make a new PEL and check new value is still there
    auto newData = pel->data();
    auto newPel = std::make_unique<PEL>(newData);
    EXPECT_EQ(newTime, newPel->commitTime());
}

TEST_F(PELTest, AssignIDTest)
{
    auto data = pelDataFactory(TestPELType::pelSimple);
    auto pel = std::make_unique<PEL>(data);

    auto origID = pel->id();
    pel->assignID();
    auto newID = pel->id();

    EXPECT_NE(origID, newID);

    // Make a new PEL and check new value is still there
    auto newData = pel->data();
    auto newPel = std::make_unique<PEL>(newData);
    EXPECT_EQ(newID, newPel->id());
}

TEST_F(PELTest, WithLogIDTest)
{
    auto data = pelDataFactory(TestPELType::pelSimple);
    auto pel = std::make_unique<PEL>(data, 0x42);

    EXPECT_TRUE(pel->valid());
    EXPECT_EQ(pel->obmcLogID(), 0x42);
}

TEST_F(PELTest, InvalidPELTest)
{
    auto data = pelDataFactory(TestPELType::pelSimple);

    // Too small
    data.resize(PrivateHeader::flattenedSize());

    auto pel = std::make_unique<PEL>(data);

    EXPECT_TRUE(pel->privateHeader().valid());
    EXPECT_FALSE(pel->userHeader().valid());
    EXPECT_FALSE(pel->valid());

    // Now corrupt the private header
    data = pelDataFactory(TestPELType::pelSimple);
    data.at(0) = 0;
    pel = std::make_unique<PEL>(data);

    EXPECT_FALSE(pel->privateHeader().valid());
    EXPECT_TRUE(pel->userHeader().valid());
    EXPECT_FALSE(pel->valid());
}

TEST_F(PELTest, EmptyDataTest)
{
    std::vector<uint8_t> data;
    auto pel = std::make_unique<PEL>(data);

    EXPECT_FALSE(pel->privateHeader().valid());
    EXPECT_FALSE(pel->userHeader().valid());
    EXPECT_FALSE(pel->valid());
}

TEST_F(PELTest, CreateFromRegistryTest)
{
    message::Entry regEntry;
    uint64_t timestamp = 5;

    regEntry.name = "test";
    regEntry.subsystem = 5;
    regEntry.actionFlags = 0xC000;
    regEntry.src.type = 0xBD;
    regEntry.src.reasonCode = 0x1234;

    std::vector<std::string> data{"KEY1=VALUE1"};
    AdditionalData ad{data};
    MockDataInterface dataIface;

    PEL pel{regEntry, 42, timestamp, phosphor::logging::Entry::Level::Error, ad,
            dataIface};

    EXPECT_TRUE(pel.valid());
    EXPECT_EQ(pel.privateHeader().obmcLogID(), 42);
    EXPECT_EQ(pel.userHeader().severity(), 0x40);

    EXPECT_EQ(pel.primarySRC().value()->asciiString(),
              "BD051234                        ");

    // Check that certain optional sections have been created
    size_t mtmsCount = 0;
    size_t euhCount = 0;
    size_t udCount = 0;

    for (const auto& section : pel.optionalSections())
    {
        if (section->header().id ==
            static_cast<uint16_t>(SectionID::failingMTMS))
        {
            mtmsCount++;
        }
        else if (section->header().id ==
                 static_cast<uint16_t>(SectionID::extendedUserHeader))
        {
            euhCount++;
        }
        else if (section->header().id ==
                 static_cast<uint16_t>(SectionID::userData))
        {
            udCount++;
        }
    }

    EXPECT_EQ(mtmsCount, 1);
    EXPECT_EQ(euhCount, 1);
    EXPECT_EQ(udCount, 2); // AD section and sysInfo section
}

// Test that we'll create Generic optional sections for sections that
// there aren't explicit classes for.
TEST_F(PELTest, GenericSectionTest)
{
    auto data = pelDataFactory(TestPELType::pelSimple);

    std::vector<uint8_t> section1{0x58, 0x58, // ID 'XX'
                                  0x00, 0x18, // Size
                                  0x01, 0x02, // version, subtype
                                  0x03, 0x04, // comp ID

                                  // some data
                                  0x20, 0x30, 0x05, 0x09, 0x11, 0x1E, 0x1, 0x63,
                                  0x20, 0x31, 0x06, 0x0F, 0x09, 0x22, 0x3A,
                                  0x00};

    std::vector<uint8_t> section2{
        0x59, 0x59, // ID 'YY'
        0x00, 0x20, // Size
        0x01, 0x02, // version, subtype
        0x03, 0x04, // comp ID

        // some data
        0x20, 0x30, 0x05, 0x09, 0x11, 0x1E, 0x1, 0x63, 0x20, 0x31, 0x06, 0x0F,
        0x09, 0x22, 0x3A, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};

    // Add the new sections at the end
    data.insert(data.end(), section1.begin(), section1.end());
    data.insert(data.end(), section2.begin(), section2.end());

    // Increment the section count
    data.at(27) += 2;
    auto origData = data;

    PEL pel{data};

    const auto& sections = pel.optionalSections();

    bool foundXX = false;
    bool foundYY = false;

    // Check that we can find these 2 Generic sections
    for (const auto& section : sections)
    {
        if (section->header().id == 0x5858)
        {
            foundXX = true;
            EXPECT_NE(dynamic_cast<Generic*>(section.get()), nullptr);
        }
        else if (section->header().id == 0x5959)
        {
            foundYY = true;
            EXPECT_NE(dynamic_cast<Generic*>(section.get()), nullptr);
        }
    }

    EXPECT_TRUE(foundXX);
    EXPECT_TRUE(foundYY);

    // Now flatten and check
    auto newData = pel.data();

    EXPECT_EQ(origData, newData);
}

// Test that an invalid section will still get a Generic object
TEST_F(PELTest, InvalidGenericTest)
{
    auto data = pelDataFactory(TestPELType::pelSimple);

    // Not a valid section
    std::vector<uint8_t> section1{0x01, 0x02, 0x03};

    data.insert(data.end(), section1.begin(), section1.end());

    // Increment the section count
    data.at(27) += 1;

    PEL pel{data};
    EXPECT_FALSE(pel.valid());

    const auto& sections = pel.optionalSections();

    bool foundGeneric = false;
    for (const auto& section : sections)
    {
        if (dynamic_cast<Generic*>(section.get()) != nullptr)
        {
            foundGeneric = true;
            EXPECT_EQ(section->valid(), false);
            break;
        }
    }

    EXPECT_TRUE(foundGeneric);
}

// Create a UserData section out of AdditionalData
TEST_F(PELTest, MakeUDSectionTest)
{
    std::vector<std::string> ad{"KEY1=VALUE1", "KEY2=VALUE2", "KEY3=VALUE3",
                                "ESEL=TEST"};
    AdditionalData additionalData{ad};

    auto ud = util::makeADUserDataSection(additionalData);

    EXPECT_TRUE(ud->valid());
    EXPECT_EQ(ud->header().id, 0x5544);
    EXPECT_EQ(ud->header().version, 0x01);
    EXPECT_EQ(ud->header().subType, 0x01);
    EXPECT_EQ(ud->header().componentID, 0x2000);

    const auto& d = ud->data();

    std::string jsonString{d.begin(), d.end()};

    std::string expectedJSON =
        R"({"KEY1":"VALUE1","KEY2":"VALUE2","KEY3":"VALUE3"})";

    // The actual data is null padded to a 4B boundary.
    std::vector<uint8_t> expectedData;
    expectedData.resize(52, '\0');
    memcpy(expectedData.data(), expectedJSON.data(), expectedJSON.size());

    EXPECT_EQ(d, expectedData);

    // Ensure we can read this as JSON
    auto newJSON = nlohmann::json::parse(jsonString);
    EXPECT_EQ(newJSON["KEY1"], "VALUE1");
    EXPECT_EQ(newJSON["KEY2"], "VALUE2");
    EXPECT_EQ(newJSON["KEY3"], "VALUE3");
}

// Create the UserData section that contains system info
TEST_F(PELTest, SysInfoSectionTest)
{
    MockDataInterface dataIface;

    EXPECT_CALL(dataIface, getBMCFWVersionID()).WillOnce(Return("ABCD1234"));
    EXPECT_CALL(dataIface, getBMCState()).WillOnce(Return("State.Ready"));
    EXPECT_CALL(dataIface, getChassisState()).WillOnce(Return("State.On"));
    EXPECT_CALL(dataIface, getHostState()).WillOnce(Return("State.Off"));

    std::string pid = "_PID=" + std::to_string(getpid());
    std::vector<std::string> ad{pid};
    AdditionalData additionalData{ad};

    auto ud = util::makeSysInfoUserDataSection(additionalData, dataIface);

    EXPECT_TRUE(ud->valid());
    EXPECT_EQ(ud->header().id, 0x5544);
    EXPECT_EQ(ud->header().version, 0x01);
    EXPECT_EQ(ud->header().subType, 0x01);
    EXPECT_EQ(ud->header().componentID, 0x2000);

    // Pull out the JSON data and check it.
    const auto& d = ud->data();
    std::string jsonString{d.begin(), d.end()};
    auto json = nlohmann::json::parse(jsonString);

    // Ensure the 'Process Name' entry contains 'pel_test'
    auto name = json["Process Name"].get<std::string>();
    EXPECT_NE(name.find("pel_test"), std::string::npos);

    auto version = json["BMC Version ID"].get<std::string>();
    EXPECT_EQ(version, "ABCD1234");

    auto state = json["BMCState"].get<std::string>();
    EXPECT_EQ(state, "Ready");

    state = json["ChassisState"].get<std::string>();
    EXPECT_EQ(state, "On");

    state = json["HostState"].get<std::string>();
    EXPECT_EQ(state, "Off");
}

// Test that the sections that override
//     virtual std::optional<std::string> Section::getJSON() const
// return valid JSON.
TEST_F(PELTest, SectionJSONTest)
{
    auto data = pelDataFactory(TestPELType::pelSimple);
    PEL pel{data};

    // Check that all JSON returned from the sections is
    // parseable by nlohmann::json, which will throw an
    // exception and fail the test if there is a problem.

    // The getJSON() response needs to be wrapped in a { } to make
    // actual valid JSON (PEL::toJSON() usually handles that).

    auto jsonString = pel.privateHeader().getJSON();

    // PrivateHeader always prints JSON
    ASSERT_TRUE(jsonString);
    *jsonString = '{' + *jsonString + '}';
    auto json = nlohmann::json::parse(*jsonString);

    jsonString = pel.userHeader().getJSON();

    // UserHeader always prints JSON
    ASSERT_TRUE(jsonString);
    *jsonString = '{' + *jsonString + '}';
    json = nlohmann::json::parse(*jsonString);

    for (const auto& section : pel.optionalSections())
    {
        // The optional sections may or may not have implemented getJSON().
        jsonString = section->getJSON();
        if (jsonString)
        {
            *jsonString = '{' + *jsonString + '}';
            auto json = nlohmann::json::parse(*jsonString);
        }
    }
}
