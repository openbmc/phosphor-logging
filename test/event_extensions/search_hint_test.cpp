#include "event_extensions/query/search_hint/extension.hpp"
#include "event_extensions/registry.hpp"

#include <sdbusplus/test/sdbus_mock.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace phosphor::logging::event_extensions
{

class SearchHintTest : public ::testing::Test
{
  protected:
    ::testing::NiceMock<sdbusplus::SdBusMock> sdbusMock;
    sdbusplus::bus_t bus = sdbusplus::get_mocked_new(&sdbusMock);
    sdbusplus::object_path objectPath{"/xyz/openbmc_project/logging/entry/1"};
    Context context{bus, objectPath};
};

TEST_F(SearchHintTest, InterfaceName)
{
    EXPECT_EQ(query::search_hint::interface,
              "xyz.openbmc_project.Logging.Extension.Query.SearchHint");
}

TEST_F(SearchHintTest, RegisterExtension)
{
    Registry registry;
    query::search_hint::registerExtension(registry);
    const auto* registration = registry.find(query::search_hint::interface);
    ASSERT_NE(registration, nullptr);

    EXPECT_TRUE(static_cast<bool>(registration->createCallback));
    EXPECT_TRUE(static_cast<bool>(registration->restoreCallback));
    EXPECT_FALSE(static_cast<bool>(registration->runtimeMetadataCallback));
}

TEST_F(SearchHintTest, CreateAndSerialize)
{
    Request request{
        .interface = std::string(query::search_hint::interface),
        .data = {{"Service", "my-service"}, {"Hint", "fan0-failed"}},
    };

    auto extension = query::search_hint::create(context, request);
    ASSERT_NE(extension, nullptr);
    EXPECT_EQ(extension->interface(), query::search_hint::interface);

    auto* searchHint =
        dynamic_cast<query::search_hint::Extension*>(extension.get());
    ASSERT_NE(searchHint, nullptr);
    EXPECT_EQ(searchHint->service(), "my-service");
    EXPECT_EQ(searchHint->hint(), "fan0-failed");

    nlohmann::json expected = {{"Service", "my-service"},
                               {"Hint", "fan0-failed"}};
    EXPECT_EQ(extension->serialize(), expected);
}

TEST_F(SearchHintTest, CreateMissingFields)
{
    Request missingHint{
        .interface = std::string(query::search_hint::interface),
        .data = {{"Service", "my-service"}},
    };
    EXPECT_EQ(query::search_hint::create(context, missingHint), nullptr);

    Request missingService{
        .interface = std::string(query::search_hint::interface),
        .data = {{"Hint", "fan0-failed"}},
    };
    EXPECT_EQ(query::search_hint::create(context, missingService), nullptr);

    Request emptyHint{
        .interface = std::string(query::search_hint::interface),
        .data = {{"Service", "my-service"}, {"Hint", ""}},
    };
    EXPECT_EQ(query::search_hint::create(context, emptyHint), nullptr);
}

TEST_F(SearchHintTest, CreateInvalidData)
{
    Request wrongType{
        .interface = std::string(query::search_hint::interface),
        .data = {{"Service", 1}, {"Hint", "fan0-failed"}},
    };
    EXPECT_EQ(query::search_hint::create(context, wrongType), nullptr);

    Request notObject{
        .interface = std::string(query::search_hint::interface),
        .data = "not-an-object",
    };
    EXPECT_EQ(query::search_hint::create(context, notObject), nullptr);
}

TEST_F(SearchHintTest, Restore)
{
    nlohmann::json data = {{"Service", "my-service"}, {"Hint", "fan0-failed"}};

    auto extension = query::search_hint::restore(context, data);
    ASSERT_NE(extension, nullptr);
    EXPECT_EQ(extension->serialize(), data);
}

TEST_F(SearchHintTest, RestoreInvalid)
{
    EXPECT_EQ(query::search_hint::restore(context, {{"Service", "my-service"}}),
              nullptr);
    EXPECT_EQ(query::search_hint::restore(
                  context, {{"Service", "my-service"}, {"Hint", 5}}),
              nullptr);
}

} // namespace phosphor::logging::event_extensions
