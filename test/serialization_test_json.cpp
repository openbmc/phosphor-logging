#include "elog_entry.hpp"
#include "elog_serialize.hpp"
#include "serialization_tests.hpp"

#include <nlohmann/json.hpp>

#include <fstream>

namespace phosphor
{
namespace logging
{
namespace test
{

// Each test needs its own temp directory since the global tmplt
// can only be used with mkdtemp once.
class TestJsonSerialization : public testing::Test
{
  public:
    TestJsonSerialization()
    {
        char tmpl[] = "/tmp/logging_json_test.XXXXXX";
        dir = fs::path(mkdtemp(tmpl));
    }

    ~TestJsonSerialization()
    {
        fs::remove_all(dir);
    }

    fs::path dir;
};

TEST_F(TestJsonSerialization, testJsonPath)
{
    auto id = 99;
    auto e = std::make_unique<Entry>(
        bus, std::string(OBJ_ENTRY) + '/' + std::to_string(id), id, manager);
    auto path = serializeJson(*e, dir);
    EXPECT_EQ(path.c_str(), dir / (std::to_string(id) + ".json"));
}

TEST_F(TestJsonSerialization, testJsonProperties)
{
    auto id = 99;
    phosphor::logging::AssociationList associations{
        {"callout", "fault", "/xyz/openbmc_project/inventory/system/chassis"}};
    std::map<std::string, std::string> testData = {{"KEY1", "value1"},
                                                   {"KEY2", "value2"}};
    uint64_t timestamp{100};
    std::string message{"test error"};
    std::string fwLevel{"level42"};
    std::string inputPath = getEntrySerializePath(id, dir);

    auto input = std::make_unique<Entry>(
        bus, std::string(OBJ_ENTRY) + '/' + std::to_string(id), id, timestamp,
        Entry::Level::Informational, std::move(message), std::move(testData),
        std::move(associations), fwLevel, inputPath, manager);

    auto jsonPath = serializeJson(*input, dir);

    EXPECT_TRUE(std::filesystem::exists(jsonPath));

    std::ifstream is(jsonPath);
    nlohmann::json j = nlohmann::json::parse(is);

    EXPECT_EQ(j["id"].get<uint32_t>(), 99);
    EXPECT_EQ(j["severity"].get<int>(),
              static_cast<int>(Entry::Level::Informational));
    EXPECT_EQ(j["timestamp"].get<uint64_t>(), 100);
    EXPECT_EQ(j["message"].get<std::string>(), "test error");
    EXPECT_EQ(j["resolved"].get<bool>(), false);
    EXPECT_EQ(j["version"].get<std::string>(), "level42");
    EXPECT_EQ(j["updateTimestamp"].get<uint64_t>(), 100);
    EXPECT_EQ(j["eventId"].get<std::string>(), "");
    EXPECT_EQ(j["resolution"].get<std::string>(), "");

    auto additionalData =
        j["additionalData"].get<std::map<std::string, std::string>>();
    EXPECT_EQ(additionalData.size(), 2);
    EXPECT_EQ(additionalData["KEY1"], "value1");
    EXPECT_EQ(additionalData["KEY2"], "value2");

    auto assocs = j["associations"];
    EXPECT_EQ(assocs.size(), 1);
    EXPECT_EQ(assocs[0][0].get<std::string>(), "callout");
    EXPECT_EQ(assocs[0][1].get<std::string>(), "fault");
    EXPECT_EQ(assocs[0][2].get<std::string>(),
              "/xyz/openbmc_project/inventory/system/chassis");
}

TEST_F(TestJsonSerialization, testJsonEmptyAdditionalData)
{
    auto id = 50;
    phosphor::logging::AssociationList associations{};
    std::map<std::string, std::string> testData{};
    uint64_t timestamp{200};
    std::string message{"empty data error"};
    std::string fwLevel{"level1"};
    std::string inputPath = getEntrySerializePath(id, dir);

    auto input = std::make_unique<Entry>(
        bus, std::string(OBJ_ENTRY) + '/' + std::to_string(id), id, timestamp,
        Entry::Level::Error, std::move(message), std::move(testData),
        std::move(associations), fwLevel, inputPath, manager);

    auto jsonPath = serializeJson(*input, dir);

    std::ifstream is(jsonPath);
    nlohmann::json j = nlohmann::json::parse(is);

    auto additionalData =
        j["additionalData"].get<std::map<std::string, std::string>>();
    EXPECT_TRUE(additionalData.empty());
    EXPECT_TRUE(j["associations"].empty());
}

TEST_F(TestJsonSerialization, testBinarySerializationUnchanged)
{
    auto id = 42;
    phosphor::logging::AssociationList associations{};
    std::map<std::string, std::string> testData = {{"FOO", "bar"}};
    uint64_t timestamp{300};
    std::string message{"binary test"};
    std::string fwLevel{"v1.0"};
    std::string inputPath = getEntrySerializePath(id, dir);

    auto input = std::make_unique<Entry>(
        bus, std::string(OBJ_ENTRY) + '/' + std::to_string(id), id, timestamp,
        Entry::Level::Warning, std::move(message), std::move(testData),
        std::move(associations), fwLevel, inputPath, manager);

    auto path = serialize(*input, dir);

    auto idStr = path.filename();
    auto outputId = std::stol(idStr.c_str());
    auto output = std::make_unique<Entry>(
        bus, std::filesystem::path(OBJ_ENTRY) / idStr, outputId, manager);
    deserialize(path, *output);

    EXPECT_EQ(input->id(), output->id());
    EXPECT_EQ(input->severity(), output->severity());
    EXPECT_EQ(input->timestamp(), output->timestamp());
    EXPECT_EQ(input->message(), output->message());
    EXPECT_EQ(input->additionalData(), output->additionalData());
    EXPECT_EQ(input->resolved(), output->resolved());
    EXPECT_EQ(input->associations(), output->associations());
    EXPECT_EQ(input->version(), output->version());
}

} // namespace test
} // namespace logging
} // namespace phosphor
