#include "event_extensions/extension.hpp"
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

ExtensionPtr createExtension(const Context&, const Request&)
{
    return std::make_unique<TestExtension>();
}

ExtensionPtr restoreExtension(const Context&, const nlohmann::json&)
{
    return std::make_unique<TestExtension>();
}

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

TEST(RegistryTest, UnknownExtensionLookup)
{
    Registry registry;
    EXPECT_EQ(registry.find("test.interface"), nullptr);
}

TEST(RegistryTest, RegisterExtension)
{
    Registry registry;

    registry.registerExtension("test.interface",
                               {
                                   .createCallback = createExtension,
                                   .restoreCallback = restoreExtension,
                               });

    EXPECT_NE(registry.find("test.interface"), nullptr);
}

TEST(RegistryTest, DuplicateExtensionRegistration)
{
    Registry registry;

    registry.registerExtension("test.interface",
                               {
                                   .createCallback = createExtension,
                                   .restoreCallback = restoreExtension,
                               });

    registry.registerExtension("test.interface",
                               {
                                   .createCallback = createExtension,
                                   .restoreCallback = restoreExtension,
                               });

    EXPECT_NE(registry.find("test.interface"), nullptr);
}

TEST(ExtensionTest, Serialize)
{
    TestExtension extension;
    auto data = extension.serialize();

    EXPECT_TRUE(data.contains("Value"));
    EXPECT_EQ(data["Value"], "test");
}

TEST(RegistryTest, RegistrationCallbacksPresent)
{
    Registry registry;
    registry.registerExtension("test.interface",
                               {
                                   .createCallback = createExtension,
                                   .restoreCallback = restoreExtension,
                               });

    const auto* registration = registry.find("test.interface");
    ASSERT_NE(registration, nullptr);

    EXPECT_TRUE(static_cast<bool>(registration->createCallback));
    EXPECT_TRUE(static_cast<bool>(registration->restoreCallback));
}

} // namespace phosphor::logging::event_extensions
