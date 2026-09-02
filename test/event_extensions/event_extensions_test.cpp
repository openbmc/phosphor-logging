#include "event_extensions/extension.hpp"
#include "event_extensions/manager.hpp"
#include "event_extensions/registry.hpp"
#include "event_extensions/request.hpp"

#include <gtest/gtest.h>

namespace phosphor::logging::event_extensions
{

namespace
{

class TestExtension : public Extension
{
  public:
    std::string_view interface() const override
    {
        return "test.interface";
    }

    nlohmann::json serialize() const override
    {
        return {
            {"Value", "test"},
        };
    }
};

} // namespace

TEST(RequestTest, DefaultConstruct)
{
    Request request;

    EXPECT_TRUE(request.interface.empty());
    EXPECT_TRUE(request.data.is_null());
}

TEST(RequestTest, EmptyRequestList)
{
    RequestList requests;

    EXPECT_TRUE(requests.empty());
}

TEST(RegistryTest, UnknownProviderLookup)
{
    Registry registry;

    EXPECT_EQ(registry.lookup("test.interface"), nullptr);
}

TEST(ExtensionTest, DefaultSerialize)
{
    TestExtension extension;
    auto data = extension.serialize();

    EXPECT_TRUE(data.contains("Value"));
    EXPECT_EQ(data["Value"], "test");
}

} // namespace phosphor::logging::event_extensions
