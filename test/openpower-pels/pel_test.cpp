// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "elog_entry.hpp"
#include "extensions/openpower-pels/generic.hpp"
#include "extensions/openpower-pels/pel.hpp"
#include "mocks.hpp"
#include "pel_utils.hpp"

#include <filesystem>
#include <fstream>
#include <optional>

#include <gtest/gtest.h>

namespace fs = std::filesystem;
using namespace openpower::pels;
using ::testing::_;
using ::testing::DoAll;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::SetArgReferee;

class PELTest : public CleanLogID
{};

fs::path makeTempDir()
{
    char path[] = "/tmp/tempdirXXXXXX";
    std::filesystem::path dir = mkdtemp(path);
    return dir;
}

int writeFileAndGetFD(const fs::path& dir, const std::vector<uint8_t>& data)
{
    static size_t count = 0;
    fs::path path = dir / (std::string{"file"} + std::to_string(count));
    std::ofstream stream{path};
    count++;

    stream.write(reinterpret_cast<const char*>(data.data()), data.size());
    stream.close();

    FILE* fp = fopen(path.c_str(), "r");
    return fileno(fp);
}

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

    std::map<std::string, std::string> data{{"KEY1", "VALUE1"}};
    AdditionalData ad{data};
    NiceMock<MockDataInterface> dataIface;
    NiceMock<MockJournal> journal;
    PelFFDC ffdc;

    PEL pel{regEntry, 42,   timestamp, phosphor::logging::Entry::Level::Error,
            ad,       ffdc, dataIface, journal};

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
    ASSERT_FALSE(pel.isHwCalloutPresent());

    {
        // The same thing, but without the action flags specified
        // in the registry, so the constructor should set them.
        regEntry.actionFlags = std::nullopt;

        PEL pel2{regEntry,  42,
                 timestamp, phosphor::logging::Entry::Level::Error,
                 ad,        ffdc,
                 dataIface, journal};

        EXPECT_EQ(pel2.userHeader().actionFlags(), 0xA800);
    }
}

// Test that when the AdditionalData size is over 16KB that
// the PEL that's created is exactly 16KB since the UserData
// section that contains all that data was pruned.
TEST_F(PELTest, CreateTooBigADTest)
{
    message::Entry regEntry;
    uint64_t timestamp = 5;

    regEntry.name = "test";
    regEntry.subsystem = 5;
    regEntry.actionFlags = 0xC000;
    regEntry.src.type = 0xBD;
    regEntry.src.reasonCode = 0x1234;
    PelFFDC ffdc;

    // Over the 16KB max PEL size
    std::map<std::string, std::string> data{{"KEY1", std::string(17000, 'G')}};
    AdditionalData ad{data};
    NiceMock<MockDataInterface> dataIface;
    NiceMock<MockJournal> journal;

    PEL pel{regEntry, 42,   timestamp, phosphor::logging::Entry::Level::Error,
            ad,       ffdc, dataIface, journal};

    EXPECT_TRUE(pel.valid());
    EXPECT_EQ(pel.size(), 16384);

    // Make sure that there are still 2 UD sections.
    const auto& optSections = pel.optionalSections();
    auto udCount = std::count_if(
        optSections.begin(), optSections.end(), [](const auto& section) {
            return section->header().id ==
                   static_cast<uint16_t>(SectionID::userData);
        });

    EXPECT_EQ(udCount, 2); // AD section and sysInfo section
}

// Test that we'll create Generic optional sections for sections that
// there aren't explicit classes for.
TEST_F(PELTest, GenericSectionTest)
{
    auto data = pelDataFactory(TestPELType::pelSimple);

    std::vector<uint8_t> section1{
        0x58, 0x58, // ID 'XX'
        0x00, 0x18, // Size
        0x01, 0x02, // version, subtype
        0x03, 0x04, // comp ID

        // some data
        0x20, 0x30, 0x05, 0x09, 0x11, 0x1E, 0x1, 0x63, 0x20, 0x31, 0x06, 0x0F,
        0x09, 0x22, 0x3A, 0x00};

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
    std::map<std::string, std::string> ad{{"KEY1", "VALUE1"},
                                          {"KEY2", "VALUE2"},
                                          {"KEY3", "VALUE3"},
                                          {"ESEL", "TEST"}};
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
    EXPECT_CALL(dataIface, getBootState())
        .WillOnce(Return("State.SystemInitComplete"));
    EXPECT_CALL(dataIface, getSystemIMKeyword())
        .WillOnce(Return(std::vector<uint8_t>{0, 1, 0x55, 0xAA}));

    std::map<std::string, std::string> ad{{"_PID", std::to_string(getpid())}};
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

    // Ensure the 'Process Name' entry contains the name of this test
    // executable.
    auto name = json["Process Name"].get<std::string>();
    auto found = (name.find("pel_test") != std::string::npos) ||
                 (name.find("test-openpower-pels-pel") != std::string::npos);
    EXPECT_TRUE(found);
    // @TODO(stwcx): remove 'pel_test' when removing autotools.

    auto version = json["FW Version ID"].get<std::string>();
    EXPECT_EQ(version, "ABCD1234");

    auto state = json["BMCState"].get<std::string>();
    EXPECT_EQ(state, "Ready");

    state = json["ChassisState"].get<std::string>();
    EXPECT_EQ(state, "On");

    state = json["HostState"].get<std::string>();
    EXPECT_EQ(state, "Off");

    state = json["BootState"].get<std::string>();
    EXPECT_EQ(state, "SystemInitComplete");

    auto keyword = json["System IM"].get<std::string>();
    EXPECT_EQ(keyword, "000155AA");
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

    auto jsonString = pel.privateHeader().getJSON('O');

    // PrivateHeader always prints JSON
    ASSERT_TRUE(jsonString);
    *jsonString = '{' + *jsonString + '}';
    auto json = nlohmann::json::parse(*jsonString);

    jsonString = pel.userHeader().getJSON('O');

    // UserHeader always prints JSON
    ASSERT_TRUE(jsonString);
    *jsonString = '{' + *jsonString + '}';
    json = nlohmann::json::parse(*jsonString);

    for (const auto& section : pel.optionalSections())
    {
        // The optional sections may or may not have implemented getJSON().
        jsonString = section->getJSON('O');
        if (jsonString)
        {
            *jsonString = '{' + *jsonString + '}';
            auto json = nlohmann::json::parse(*jsonString);
        }
    }
}

PelFFDCfile getJSONFFDC(const fs::path& dir)
{
    PelFFDCfile ffdc;
    ffdc.format = UserDataFormat::json;
    ffdc.subType = 5;
    ffdc.version = 42;

    auto inputJSON = R"({
        "key1": "value1",
        "key2": 42,
        "key3" : [1, 2, 3, 4, 5],
        "key4": {"key5": "value5"}
    })"_json;

    // Write the JSON to a file and get its descriptor.
    auto s = inputJSON.dump();
    std::vector<uint8_t> data{s.begin(), s.end()};
    ffdc.fd = writeFileAndGetFD(dir, data);

    return ffdc;
}

TEST_F(PELTest, MakeJSONFileUDSectionTest)
{
    auto dir = makeTempDir();

    {
        auto ffdc = getJSONFFDC(dir);

        auto ud = util::makeFFDCuserDataSection(0x2002, ffdc);
        close(ffdc.fd);
        ASSERT_TRUE(ud);
        ASSERT_TRUE(ud->valid());
        EXPECT_EQ(ud->header().id, 0x5544);

        EXPECT_EQ(ud->header().version,
                  static_cast<uint8_t>(UserDataFormatVersion::json));
        EXPECT_EQ(ud->header().subType,
                  static_cast<uint8_t>(UserDataFormat::json));
        EXPECT_EQ(ud->header().componentID,
                  static_cast<uint16_t>(ComponentID::phosphorLogging));

        // Pull the JSON back out of the the UserData section
        const auto& d = ud->data();
        std::string js{d.begin(), d.end()};
        auto json = nlohmann::json::parse(js);

        EXPECT_EQ("value1", json["key1"].get<std::string>());
        EXPECT_EQ(42, json["key2"].get<int>());

        std::vector<int> key3Values{1, 2, 3, 4, 5};
        EXPECT_EQ(key3Values, json["key3"].get<std::vector<int>>());

        std::map<std::string, std::string> key4Values{{"key5", "value5"}};
        auto actual = json["key4"].get<std::map<std::string, std::string>>();
        EXPECT_EQ(key4Values, actual);
    }

    {
        // A bad FD
        PelFFDCfile ffdc;
        ffdc.format = UserDataFormat::json;
        ffdc.subType = 5;
        ffdc.version = 42;
        ffdc.fd = 10000;

        // The section shouldn't get made
        auto ud = util::makeFFDCuserDataSection(0x2002, ffdc);
        ASSERT_FALSE(ud);
    }

    fs::remove_all(dir);
}

PelFFDCfile getCBORFFDC(const fs::path& dir)
{
    PelFFDCfile ffdc;
    ffdc.format = UserDataFormat::cbor;
    ffdc.subType = 5;
    ffdc.version = 42;

    auto inputJSON = R"({
        "key1": "value1",
        "key2": 42,
        "key3" : [1, 2, 3, 4, 5],
        "key4": {"key5": "value5"}
    })"_json;

    // Convert the JSON to CBOR and write it to a file
    auto data = nlohmann::json::to_cbor(inputJSON);
    ffdc.fd = writeFileAndGetFD(dir, data);

    return ffdc;
}

TEST_F(PELTest, MakeCBORFileUDSectionTest)
{
    auto dir = makeTempDir();

    auto ffdc = getCBORFFDC(dir);
    auto ud = util::makeFFDCuserDataSection(0x2002, ffdc);
    close(ffdc.fd);
    ASSERT_TRUE(ud);
    ASSERT_TRUE(ud->valid());
    EXPECT_EQ(ud->header().id, 0x5544);

    EXPECT_EQ(ud->header().version,
              static_cast<uint8_t>(UserDataFormatVersion::cbor));
    EXPECT_EQ(ud->header().subType, static_cast<uint8_t>(UserDataFormat::cbor));
    EXPECT_EQ(ud->header().componentID,
              static_cast<uint16_t>(ComponentID::phosphorLogging));

    // Pull the CBOR back out of the PEL section
    // The number of pad bytes to make the section be 4B aligned
    // was added at the end, read it and then remove it and the
    // padding before parsing it.
    auto data = ud->data();
    Stream stream{data};
    stream.offset(data.size() - 4);
    uint32_t pad;
    stream >> pad;

    data.resize(data.size() - 4 - pad);

    auto json = nlohmann::json::from_cbor(data);

    EXPECT_EQ("value1", json["key1"].get<std::string>());
    EXPECT_EQ(42, json["key2"].get<int>());

    std::vector<int> key3Values{1, 2, 3, 4, 5};
    EXPECT_EQ(key3Values, json["key3"].get<std::vector<int>>());

    std::map<std::string, std::string> key4Values{{"key5", "value5"}};
    auto actual = json["key4"].get<std::map<std::string, std::string>>();
    EXPECT_EQ(key4Values, actual);

    fs::remove_all(dir);
}

PelFFDCfile getTextFFDC(const fs::path& dir)
{
    PelFFDCfile ffdc;
    ffdc.format = UserDataFormat::text;
    ffdc.subType = 5;
    ffdc.version = 42;

    std::string text{"this is some text that will be used for FFDC"};
    std::vector<uint8_t> data{text.begin(), text.end()};

    ffdc.fd = writeFileAndGetFD(dir, data);

    return ffdc;
}

TEST_F(PELTest, MakeTextFileUDSectionTest)
{
    auto dir = makeTempDir();

    auto ffdc = getTextFFDC(dir);
    auto ud = util::makeFFDCuserDataSection(0x2002, ffdc);
    close(ffdc.fd);
    ASSERT_TRUE(ud);
    ASSERT_TRUE(ud->valid());
    EXPECT_EQ(ud->header().id, 0x5544);

    EXPECT_EQ(ud->header().version,
              static_cast<uint8_t>(UserDataFormatVersion::text));
    EXPECT_EQ(ud->header().subType, static_cast<uint8_t>(UserDataFormat::text));
    EXPECT_EQ(ud->header().componentID,
              static_cast<uint16_t>(ComponentID::phosphorLogging));

    // Get the text back out
    std::string text{ud->data().begin(), ud->data().end()};
    EXPECT_EQ(text, "this is some text that will be used for FFDC");

    fs::remove_all(dir);
}

PelFFDCfile getCustomFFDC(const fs::path& dir, const std::vector<uint8_t>& data)
{
    PelFFDCfile ffdc;
    ffdc.format = UserDataFormat::custom;
    ffdc.subType = 5;
    ffdc.version = 42;

    ffdc.fd = writeFileAndGetFD(dir, data);

    return ffdc;
}

TEST_F(PELTest, MakeCustomFileUDSectionTest)
{
    auto dir = makeTempDir();

    {
        std::vector<uint8_t> data{1, 2, 3, 4, 5, 6, 7, 8};

        auto ffdc = getCustomFFDC(dir, data);
        auto ud = util::makeFFDCuserDataSection(0x2002, ffdc);
        close(ffdc.fd);
        ASSERT_TRUE(ud);
        ASSERT_TRUE(ud->valid());
        EXPECT_EQ(ud->header().size, 8 + 8); // data size + header size
        EXPECT_EQ(ud->header().id, 0x5544);

        EXPECT_EQ(ud->header().version, 42);
        EXPECT_EQ(ud->header().subType, 5);
        EXPECT_EQ(ud->header().componentID, 0x2002);

        // Get the data back out
        std::vector<uint8_t> newData{ud->data().begin(), ud->data().end()};
        EXPECT_EQ(data, newData);
    }

    // Do the same thing again, but make it be non 4B aligned
    // so the data gets padded.
    {
        std::vector<uint8_t> data{1, 2, 3, 4, 5, 6, 7, 8, 9};

        auto ffdc = getCustomFFDC(dir, data);
        auto ud = util::makeFFDCuserDataSection(0x2002, ffdc);
        close(ffdc.fd);
        ASSERT_TRUE(ud);
        ASSERT_TRUE(ud->valid());
        EXPECT_EQ(ud->header().size, 12 + 8); // data size + header size
        EXPECT_EQ(ud->header().id, 0x5544);

        EXPECT_EQ(ud->header().version, 42);
        EXPECT_EQ(ud->header().subType, 5);
        EXPECT_EQ(ud->header().componentID, 0x2002);

        // Get the data back out
        std::vector<uint8_t> newData{ud->data().begin(), ud->data().end()};

        // pad the original to 12B so we can compare
        data.push_back(0);
        data.push_back(0);
        data.push_back(0);

        EXPECT_EQ(data, newData);
    }

    fs::remove_all(dir);
}

// Test Adding FFDC from files to a PEL
TEST_F(PELTest, CreateWithFFDCTest)
{
    auto dir = makeTempDir();
    message::Entry regEntry;
    uint64_t timestamp = 5;

    regEntry.name = "test";
    regEntry.subsystem = 5;
    regEntry.actionFlags = 0xC000;
    regEntry.src.type = 0xBD;
    regEntry.src.reasonCode = 0x1234;

    std::map<std::string, std::string> additionalData{{"KEY1", "VALUE1"}};
    AdditionalData ad{additionalData};
    NiceMock<MockDataInterface> dataIface;
    NiceMock<MockJournal> journal;
    PelFFDC ffdc;

    std::vector<uint8_t> customData{1, 2, 3, 4, 5, 6, 7, 8};

    // This will be trimmed when added
    std::vector<uint8_t> hugeCustomData(17000, 0x42);

    ffdc.emplace_back(getJSONFFDC(dir));
    ffdc.emplace_back(getCBORFFDC(dir));
    ffdc.emplace_back(getTextFFDC(dir));
    ffdc.emplace_back(getCustomFFDC(dir, customData));
    ffdc.emplace_back(getCustomFFDC(dir, hugeCustomData));

    PEL pel{regEntry, 42,   timestamp, phosphor::logging::Entry::Level::Error,
            ad,       ffdc, dataIface, journal};

    EXPECT_TRUE(pel.valid());

    // Clipped to the max
    EXPECT_EQ(pel.size(), 16384);

    // Check for the FFDC sections
    size_t udCount = 0;
    Section* ud = nullptr;

    for (const auto& section : pel.optionalSections())
    {
        if (section->header().id == static_cast<uint16_t>(SectionID::userData))
        {
            udCount++;
            ud = section.get();
        }
    }

    EXPECT_EQ(udCount, 7); // AD section, sysInfo, 5 ffdc sections

    // Check the last section was trimmed to
    // something a bit less that 17000.
    EXPECT_GT(ud->header().size, 14000);
    EXPECT_LT(ud->header().size, 16000);

    fs::remove_all(dir);
}

// Create a PEL with device callouts
TEST_F(PELTest, CreateWithDevCalloutsTest)
{
    message::Entry regEntry;
    uint64_t timestamp = 5;

    regEntry.name = "test";
    regEntry.subsystem = 5;
    regEntry.actionFlags = 0xC000;
    regEntry.src.type = 0xBD;
    regEntry.src.reasonCode = 0x1234;

    NiceMock<MockDataInterface> dataIface;
    NiceMock<MockJournal> journal;
    PelFFDC ffdc;

    const auto calloutJSON = R"(
    {
        "I2C":
        {
            "14":
            {
                "114":
                {
                    "Callouts":[
                        {
                        "Name": "/chassis/motherboard/cpu0",
                        "LocationCode": "P1",
                        "Priority": "H"
                        }
                    ],
                    "Dest": "proc 0 target"
                }
            }
        }
    })";

    std::vector<std::string> names{"systemA"};
    EXPECT_CALL(dataIface, getSystemNames)
        .Times(2)
        .WillRepeatedly(Return(names));

    EXPECT_CALL(dataIface, expandLocationCode("P1", 0))
        .Times(1)
        .WillOnce(Return("UXXX-P1"));

    EXPECT_CALL(dataIface, getInventoryFromLocCode("P1", 0, false))
        .WillOnce(Return(std::vector<std::string>{
            "/xyz/openbmc_project/inventory/chassis/motherboard/cpu0"}));

    EXPECT_CALL(dataIface,
                getHWCalloutFields(
                    "/xyz/openbmc_project/inventory/chassis/motherboard/cpu0",
                    _, _, _))
        .WillOnce(DoAll(SetArgReferee<1>("1234567"), SetArgReferee<2>("CCCC"),
                        SetArgReferee<3>("123456789ABC")));

    auto dataPath = getPELReadOnlyDataPath();
    std::ofstream file{dataPath / "systemA_dev_callouts.json"};
    file << calloutJSON;
    file.close();

    {
        std::map<std::string, std::string> data{
            {"CALLOUT_ERRNO", "5"},
            {"CALLOUT_DEVICE_PATH",
             "/sys/devices/platform/ahb/ahb:apb/ahb:apb:bus@1e78a000/1e78a340.i2c-bus/i2c-14/14-0072"}};

        AdditionalData ad{data};

        PEL pel{regEntry,  42,
                timestamp, phosphor::logging::Entry::Level::Error,
                ad,        ffdc,
                dataIface, journal};

        ASSERT_TRUE(pel.primarySRC().value()->callouts());
        auto& callouts = pel.primarySRC().value()->callouts()->callouts();
        ASSERT_EQ(callouts.size(), 1);
        ASSERT_TRUE(pel.isHwCalloutPresent());

        EXPECT_EQ(callouts[0]->priority(), 'H');
        EXPECT_EQ(callouts[0]->locationCode(), "UXXX-P1");

        auto& fru = callouts[0]->fruIdentity();
        EXPECT_EQ(fru->getPN().value(), "1234567");
        EXPECT_EQ(fru->getCCIN().value(), "CCCC");
        EXPECT_EQ(fru->getSN().value(), "123456789ABC");

        const auto& section = pel.optionalSections().back();

        ASSERT_EQ(section->header().id, 0x5544); // UD
        auto ud = static_cast<UserData*>(section.get());

        // Check that there was a UserData section added that
        // contains debug details about the device.
        const auto& d = ud->data();
        std::string jsonString{d.begin(), d.end()};
        auto actualJSON = nlohmann::json::parse(jsonString);

        auto expectedJSON = R"(
            {
                "PEL Internal Debug Data": {
                    "SRC": [
                      "I2C: bus: 14 address: 114 dest: proc 0 target"
                    ]
                }
            }
        )"_json;

        EXPECT_TRUE(
            actualJSON.contains("/PEL Internal Debug Data/SRC"_json_pointer));
        EXPECT_EQ(actualJSON["PEL Internal Debug Data"]["SRC"],
                  expectedJSON["PEL Internal Debug Data"]["SRC"]);
    }

    {
        // Device path not found (wrong i2c addr), so no callouts
        std::map<std::string, std::string> data{
            {"CALLOUT_ERRNO", "5"},
            {"CALLOUT_DEVICE_PATH",
             "/sys/devices/platform/ahb/ahb:apb/ahb:apb:bus@1e78a000/1e78a340.i2c-bus/i2c-14/14-0099"}};

        AdditionalData ad{data};

        PEL pel{regEntry,  42,
                timestamp, phosphor::logging::Entry::Level::Error,
                ad,        ffdc,
                dataIface, journal};

        // no callouts
        EXPECT_FALSE(pel.primarySRC().value()->callouts());

        // Now check that there was a UserData section
        // that contains the lookup error.
        const auto& section = pel.optionalSections().back();

        ASSERT_EQ(section->header().id, 0x5544); // UD
        auto ud = static_cast<UserData*>(section.get());

        const auto& d = ud->data();

        std::string jsonString{d.begin(), d.end()};

        auto actualJSON = nlohmann::json::parse(jsonString);

        auto expectedJSON =
            "{\"PEL Internal Debug Data\":{\"SRC\":"
            "[\"Problem looking up I2C callouts on 14 153: "
            "[json.exception.out_of_range.403] key '153' not found\"]}}"_json;

        EXPECT_TRUE(
            actualJSON.contains("/PEL Internal Debug Data/SRC"_json_pointer));
        EXPECT_EQ(actualJSON["PEL Internal Debug Data"]["SRC"],
                  expectedJSON["PEL Internal Debug Data"]["SRC"]);
    }

    fs::remove_all(dataPath);
}

// Test PELs when the callouts are passed in using a JSON file.
TEST_F(PELTest, CreateWithJSONCalloutsTest)
{
    PelFFDCfile ffdcFile;
    ffdcFile.format = UserDataFormat::json;
    ffdcFile.subType = 0xCA; // Callout JSON
    ffdcFile.version = 1;

    // Write these callouts to a JSON file and pass it into
    // the PEL as an FFDC file. Also has a duplicate that
    // will be removed.
    auto inputJSON = R"([
        {
            "Priority": "H",
            "LocationCode": "P0-C1"
        },
        {
            "Priority": "M",
            "Procedure": "PROCEDURE"
        },
        {
            "Priority": "L",
            "Procedure": "PROCEDURE"
        }
    ])"_json;

    auto s = inputJSON.dump();
    std::vector<uint8_t> data{s.begin(), s.end()};
    auto dir = makeTempDir();
    ffdcFile.fd = writeFileAndGetFD(dir, data);

    PelFFDC ffdc;
    ffdc.push_back(std::move(ffdcFile));

    AdditionalData ad;
    NiceMock<MockDataInterface> dataIface;
    NiceMock<MockJournal> journal;

    EXPECT_CALL(dataIface, expandLocationCode("P0-C1", 0))
        .Times(1)
        .WillOnce(Return("UXXX-P0-C1"));
    EXPECT_CALL(dataIface, getInventoryFromLocCode("P0-C1", 0, false))
        .Times(1)
        .WillOnce(Return(
            std::vector<std::string>{"/inv/system/chassis/motherboard/bmc"}));
    EXPECT_CALL(dataIface, getHWCalloutFields(
                               "/inv/system/chassis/motherboard/bmc", _, _, _))
        .Times(1)
        .WillOnce(DoAll(SetArgReferee<1>("1234567"), SetArgReferee<2>("CCCC"),
                        SetArgReferee<3>("123456789ABC")));

    message::Entry regEntry;
    regEntry.name = "test";
    regEntry.subsystem = 5;
    regEntry.actionFlags = 0xC000;
    regEntry.src.type = 0xBD;
    regEntry.src.reasonCode = 0x1234;

    PEL pel{regEntry, 42,   5,         phosphor::logging::Entry::Level::Error,
            ad,       ffdc, dataIface, journal};

    ASSERT_TRUE(pel.valid());
    ASSERT_TRUE(pel.primarySRC().value()->callouts());
    const auto& callouts = pel.primarySRC().value()->callouts()->callouts();
    ASSERT_EQ(callouts.size(), 2);
    ASSERT_TRUE(pel.isHwCalloutPresent());

    {
        EXPECT_EQ(callouts[0]->priority(), 'H');
        EXPECT_EQ(callouts[0]->locationCode(), "UXXX-P0-C1");

        auto& fru = callouts[0]->fruIdentity();
        EXPECT_EQ(fru->getPN().value(), "1234567");
        EXPECT_EQ(fru->getCCIN().value(), "CCCC");
        EXPECT_EQ(fru->getSN().value(), "123456789ABC");
        EXPECT_EQ(fru->failingComponentType(), src::FRUIdentity::hardwareFRU);
    }
    {
        EXPECT_EQ(callouts[1]->priority(), 'M');
        EXPECT_EQ(callouts[1]->locationCode(), "");

        auto& fru = callouts[1]->fruIdentity();
        EXPECT_EQ(fru->getMaintProc().value(), "PROCEDU");
        EXPECT_EQ(fru->failingComponentType(),
                  src::FRUIdentity::maintenanceProc);
    }
    fs::remove_all(dir);
}

// Test PELs with symbolic FRU callout.
TEST_F(PELTest, CreateWithJSONSymblicCalloutTest)
{
    PelFFDCfile ffdcFile;
    ffdcFile.format = UserDataFormat::json;
    ffdcFile.subType = 0xCA; // Callout JSON
    ffdcFile.version = 1;

    // Write these callouts to a JSON file and pass it into
    // the PEL as an FFDC file.
    auto inputJSON = R"([
        {
            "Priority": "M",
            "Procedure": "SVCDOCS"
        }
    ])"_json;

    auto s = inputJSON.dump();
    std::vector<uint8_t> data{s.begin(), s.end()};
    auto dir = makeTempDir();
    ffdcFile.fd = writeFileAndGetFD(dir, data);

    PelFFDC ffdc;
    ffdc.push_back(std::move(ffdcFile));

    AdditionalData ad;
    NiceMock<MockDataInterface> dataIface;
    NiceMock<MockJournal> journal;

    message::Entry regEntry;
    regEntry.name = "test";
    regEntry.subsystem = 5;
    regEntry.actionFlags = 0xC000;
    regEntry.src.type = 0xBD;
    regEntry.src.reasonCode = 0x1234;

    PEL pel{regEntry, 42,   5,         phosphor::logging::Entry::Level::Error,
            ad,       ffdc, dataIface, journal};

    ASSERT_TRUE(pel.valid());
    ASSERT_TRUE(pel.primarySRC().value()->callouts());
    const auto& callouts = pel.primarySRC().value()->callouts()->callouts();
    ASSERT_EQ(callouts.size(), 1);
    ASSERT_FALSE(pel.isHwCalloutPresent());

    {
        EXPECT_EQ(callouts[0]->priority(), 'M');
        EXPECT_EQ(callouts[0]->locationCode(), "");

        auto& fru = callouts[0]->fruIdentity();
        EXPECT_EQ(fru->getMaintProc().value(), "SVCDOCS");
    }
    fs::remove_all(dir);
}

TEST_F(PELTest, FlattenLinesTest)
{
    std::vector<std::string> msgs{"test1 test2", "test3 test4", "test5 test6"};

    auto buffer = util::flattenLines(msgs);

    std::string string{"test1 test2\ntest3 test4\ntest5 test6\n"};
    std::vector<uint8_t> expected(string.begin(), string.end());

    EXPECT_EQ(buffer, expected);
}

void checkJournalSection(const std::unique_ptr<Section>& section,
                         const std::string& expected)
{
    ASSERT_EQ(SectionID::userData,
              static_cast<SectionID>(section->header().id));
    ASSERT_EQ(UserDataFormat::text,
              static_cast<UserDataFormat>(section->header().subType));
    ASSERT_EQ(section->header().version,
              static_cast<uint8_t>(UserDataFormatVersion::text));

    auto ud = static_cast<UserData*>(section.get());

    std::vector<uint8_t> expectedData(expected.begin(), expected.end());

    // PEL sections are 4B aligned so add padding before the compare
    while (expectedData.size() % 4 != 0)
    {
        expectedData.push_back('\0');
    }

    EXPECT_EQ(ud->data(), expectedData);
}

TEST_F(PELTest, CaptureJournalTest)
{
    message::Entry regEntry;
    uint64_t timestamp = 5;

    regEntry.name = "test";
    regEntry.subsystem = 5;
    regEntry.actionFlags = 0xC000;
    regEntry.src.type = 0xBD;
    regEntry.src.reasonCode = 0x1234;

    std::map<std::string, std::string> data{};
    AdditionalData ad{data};
    NiceMock<MockDataInterface> dataIface;
    NiceMock<MockJournal> journal;
    PelFFDC ffdc;

    size_t pelSectsWithOneUD{0};

    {
        // Capture 5 lines from the journal into a single UD section
        message::JournalCapture jc = size_t{5};
        regEntry.journalCapture = jc;

        std::vector<std::string> msgs{"test1 test2", "test3 test4",
                                      "test5 test6", "4", "5"};

        EXPECT_CALL(journal, getMessages("", 5)).WillOnce(Return(msgs));

        PEL pel{regEntry,  42,
                timestamp, phosphor::logging::Entry::Level::Error,
                ad,        ffdc,
                dataIface, journal};

        // Check the generated UserData section
        std::string expected{"test1 test2\ntest3 test4\ntest5 test6\n4\n5\n"};

        checkJournalSection(pel.optionalSections().back(), expected);

        // Save for upcoming testcases
        pelSectsWithOneUD = pel.privateHeader().sectionCount();
    }

    {
        // Attempt to capture too many journal entries so the
        // section gets dropped.
        message::JournalCapture jc = size_t{1};
        regEntry.journalCapture = jc;

        EXPECT_CALL(journal, sync()).Times(1);

        // A 20000 byte line won't fit in a PEL
        EXPECT_CALL(journal, getMessages("", 1))
            .WillOnce(
                Return(std::vector<std::string>{std::string(20000, 'x')}));

        PEL pel{regEntry,  42,
                timestamp, phosphor::logging::Entry::Level::Error,
                ad,        ffdc,
                dataIface, journal};

        // Check for 1 fewer sections than in the previous PEL
        EXPECT_EQ(pel.privateHeader().sectionCount(), pelSectsWithOneUD - 1);
    }

    // Capture 3 different journal sections
    {
        message::AppCaptureList captureList{
            message::AppCapture{"app1", 3},
            message::AppCapture{"app2", 4},
            message::AppCapture{"app3", 1},
        };
        message::JournalCapture jc = captureList;
        regEntry.journalCapture = jc;

        std::vector<std::string> app1{"A B", "C D", "E F"};
        std::vector<std::string> app2{"1 2", "3 4", "5 6", "7 8"};
        std::vector<std::string> app3{"a b c"};

        std::string expected1{"A B\nC D\nE F\n"};
        std::string expected2{"1 2\n3 4\n5 6\n7 8\n"};
        std::string expected3{"a b c\n"};

        EXPECT_CALL(journal, sync()).Times(1);
        EXPECT_CALL(journal, getMessages("app1", 3)).WillOnce(Return(app1));
        EXPECT_CALL(journal, getMessages("app2", 4)).WillOnce(Return(app2));
        EXPECT_CALL(journal, getMessages("app3", 1)).WillOnce(Return(app3));

        PEL pel{regEntry,  42,
                timestamp, phosphor::logging::Entry::Level::Error,
                ad,        ffdc,
                dataIface, journal};

        // Two more sections than the 1 extra UD section in the first
        // testcase
        ASSERT_EQ(pel.privateHeader().sectionCount(), pelSectsWithOneUD + 2);

        const auto& optionalSections = pel.optionalSections();
        auto numOptSections = optionalSections.size();

        checkJournalSection(optionalSections[numOptSections - 3], expected1);
        checkJournalSection(optionalSections[numOptSections - 2], expected2);
        checkJournalSection(optionalSections[numOptSections - 1], expected3);
    }

    {
        // One section gets saved, and one is too big and gets dropped
        message::AppCaptureList captureList{
            message::AppCapture{"app4", 2},
            message::AppCapture{"app5", 1},
        };
        message::JournalCapture jc = captureList;
        regEntry.journalCapture = jc;

        std::vector<std::string> app4{"w x", "y z"};
        std::string expected4{"w x\ny z\n"};

        EXPECT_CALL(journal, sync()).Times(1);

        EXPECT_CALL(journal, getMessages("app4", 2)).WillOnce(Return(app4));

        // A 20000 byte line won't fit in a PEL
        EXPECT_CALL(journal, getMessages("app5", 1))
            .WillOnce(
                Return(std::vector<std::string>{std::string(20000, 'x')}));

        PEL pel{regEntry,  42,
                timestamp, phosphor::logging::Entry::Level::Error,
                ad,        ffdc,
                dataIface, journal};

        // The last section should have been dropped, so same as first TC
        ASSERT_EQ(pel.privateHeader().sectionCount(), pelSectsWithOneUD);

        checkJournalSection(pel.optionalSections().back(), expected4);
    }
}

// API to collect and parse the User Data section of the PEL.
nlohmann::json getDIMMInfo(const auto& pel)
{
    nlohmann::json dimmInfo{};
    auto hasDIMMInfo = [&dimmInfo](const auto& optionalSection) {
        if (optionalSection->header().id !=
            static_cast<uint16_t>(SectionID::userData))
        {
            return false;
        }
        else
        {
            auto userData = static_cast<UserData*>(optionalSection.get());

            // convert the userdata section to string and then parse in to
            // json format
            std::string userDataString{userData->data().begin(),
                                       userData->data().end()};
            nlohmann::json userDataJson = nlohmann::json::parse(userDataString);

            if (userDataJson.contains("DIMMs Additional Info"))
            {
                dimmInfo = userDataJson.at("DIMMs Additional Info");
            }
            else if (
                userDataJson.contains(
                    "/PEL Internal Debug Data/DIMMs Info Fetch Error"_json_pointer))
            {
                dimmInfo = userDataJson.at(
                    "/PEL Internal Debug Data/DIMMs Info Fetch Error"_json_pointer);
            }
            else
            {
                return false;
            }
            return true;
        }
    };
    std::ranges::any_of(pel.optionalSections(), hasDIMMInfo);

    return dimmInfo;
}

// Test whether the DIMM callouts manufacturing info is getting added to the
// SysInfo User Data section of the PEL
TEST_F(PELTest, TestDimmsCalloutInfo)
{
    {
        message::Entry entry;
        uint64_t timestamp = 5;
        AdditionalData ad;
        NiceMock<MockDataInterface> dataIface;
        NiceMock<MockJournal> journal;
        PelFFDC ffdc;

        // When callouts contain DIMM callouts.
        entry.callouts = R"(
        [
            {
                "CalloutList": [
                    {
                        "Priority": "high",
                        "LocCode": "P0-DIMM0"
                    },
                    {
                        "Priority": "low",
                        "LocCode": "P0-DIMM1"
                    }
                ]
            }
        ]
        )"_json;

        EXPECT_CALL(dataIface, expandLocationCode("P0-DIMM0", 0))
            .WillOnce(Return("U98D-P0-DIMM0"));
        EXPECT_CALL(dataIface, expandLocationCode("P0-DIMM1", 0))
            .WillOnce(Return("U98D-P0-DIMM1"));

        EXPECT_CALL(dataIface, getInventoryFromLocCode("P0-DIMM0", 0, false))
            .WillOnce(Return(std::vector<std::string>{
                "/xyz/openbmc_project/inventory/system/chassis/motherboard/dimm0"}));
        EXPECT_CALL(dataIface, getInventoryFromLocCode("P0-DIMM1", 0, false))
            .WillOnce(Return(std::vector<std::string>{
                "/xyz/openbmc_project/inventory/system/chassis/motherboard/dimm1"}));

        std::vector<uint8_t> diValue{128, 74};
        EXPECT_CALL(dataIface, getDIProperty("U98D-P0-DIMM0"))
            .WillOnce(Return(diValue));
        EXPECT_CALL(dataIface, getDIProperty("U98D-P0-DIMM1"))
            .WillOnce(Return(diValue));

        // Add some location code in expanded format to DIMM cache memory
        dataIface.addDIMMLocCode("U98D-P0-DIMM0", true);
        dataIface.addDIMMLocCode("U98D-P0-DIMM1", true);

        PEL pel{entry, 42,   timestamp, phosphor::logging::Entry::Level::Error,
                ad,    ffdc, dataIface, journal};
        nlohmann::json dimmInfoJson = getDIMMInfo(pel);

        nlohmann::json expected_data = R"(
        [
            {
                "Location Code": "U98D-P0-DIMM0",
                "DRAM Manufacturer ID": [
                    "0x80",
                    "0x4a"
                ]
            },
            {
                "Location Code": "U98D-P0-DIMM1",
                "DRAM Manufacturer ID": [
                    "0x80",
                    "0x4a"
                ]
            }
        ]
        )"_json;
        EXPECT_EQ(expected_data, dimmInfoJson);
    }
}

// When PEL has FRU callouts but PHAL is not enabled.
TEST_F(PELTest, TestNoDimmsCallout)
{
    message::Entry entry;
    uint64_t timestamp = 5;
    AdditionalData ad;
    NiceMock<MockDataInterface> dataIface;
    NiceMock<MockJournal> journal;
    PelFFDC ffdc;

    entry.callouts = R"(
        [
            {
                "CalloutList": [
                    {
                        "Priority": "high",
                        "LocCode": "P0-PROC0"
                    }
                ]
            }
        ]
        )"_json;

    EXPECT_CALL(dataIface, expandLocationCode("P0-PROC0", 0))
        .WillOnce(Return("U98D-P0-PROC0"));

    EXPECT_CALL(dataIface, getInventoryFromLocCode("P0-PROC0", 0, false))
        .WillOnce(Return(std::vector<std::string>{
            "/xyz/openbmc_project/inventory/system/chassis/motherboard/dcm0/cpu0"}));

    // Add some location code in expanded format to DIMM cache memory
    dataIface.addDIMMLocCode("U98D-P0-PROC0", false);

    PEL pel{entry, 42,   timestamp, phosphor::logging::Entry::Level::Error,
            ad,    ffdc, dataIface, journal};

    nlohmann::json dimmInfoJson = getDIMMInfo(pel);

    nlohmann::json expected_data{};

    EXPECT_EQ(expected_data, dimmInfoJson);
}

// When the PEL doesn't contain any type of callouts
TEST_F(PELTest, TestDimmsCalloutInfoWithNoCallouts)
{
    message::Entry entry;
    uint64_t timestamp = 5;
    AdditionalData ad;
    NiceMock<MockDataInterface> dataIface;
    NiceMock<MockJournal> journal;
    PelFFDC ffdc;

    PEL pel{entry, 42,   timestamp, phosphor::logging::Entry::Level::Error,
            ad,    ffdc, dataIface, journal};

    nlohmann::json dimmInfoJson = getDIMMInfo(pel);

    nlohmann::json expected_data{};

    EXPECT_EQ(expected_data, dimmInfoJson);
}

// When the PEL has DIMM callouts, but failed to fetch DI property value
TEST_F(PELTest, TestDimmsCalloutInfoDIFailure)
{
    {
        message::Entry entry;
        uint64_t timestamp = 5;
        AdditionalData ad;
        NiceMock<MockDataInterface> dataIface;
        NiceMock<MockJournal> journal;
        PelFFDC ffdc;

        entry.callouts = R"(
        [
            {
                "CalloutList": [
                    {
                        "Priority": "high",
                        "LocCode": "P0-DIMM0"
                    }
                ]
            }
        ]
        )"_json;

        EXPECT_CALL(dataIface, expandLocationCode("P0-DIMM0", 0))
            .WillOnce(Return("U98D-P0-DIMM0"));

        EXPECT_CALL(dataIface, getInventoryFromLocCode("P0-DIMM0", 0, false))
            .WillOnce(Return(std::vector<std::string>{
                "/xyz/openbmc_project/inventory/system/chassis/motherboard/dimm0"}));

        EXPECT_CALL(dataIface, getDIProperty("U98D-P0-DIMM0"))
            .WillOnce(Return(std::nullopt));

        // Add some location code in expanded format to DIMM cache memory
        dataIface.addDIMMLocCode("U98D-P0-DIMM0", true);

        PEL pel{entry, 42,   timestamp, phosphor::logging::Entry::Level::Error,
                ad,    ffdc, dataIface, journal};

        nlohmann::json dimmInfoJson = getDIMMInfo(pel);

        nlohmann::json expected_data = R"(
            [
                "Failed reading DI property from VINI Interface for the LocationCode:[U98D-P0-DIMM0]"
            ]
        )"_json;

        EXPECT_EQ(expected_data, dimmInfoJson);
    }
}
