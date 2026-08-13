#include "plugin/plugin_factory.hpp"
#include "plugin/plugin_registry.hpp"

#include <gtest/gtest.h>

namespace phosphor::logging
{

namespace
{

constexpr auto testInterface = "xyz.openbmc_project.Logging.Test";

class TestDescriptor : public plugin::Descriptor
{
  public:
    std::string_view interface() const override
    {
        return testInterface;
    }
};

class TestFactory : public PluginFactory
{
  public:
    plugin::DescriptorPtr createDescriptor(
        const plugin::Request& request) const override
    {
        (void)request;

        return std::make_unique<TestDescriptor>();
    }

    PluginPtr create(const PluginContext& context,
                     const plugin::Descriptor& descriptor) const override
    {
        (void)context;
        (void)descriptor;

        return nullptr;
    }
};

class PayloadFactory : public PluginFactory
{
  public:
    std::unique_ptr<plugin::Descriptor> createDescriptor(
        const plugin::Request&) const override
    {
        return nullptr;
    }

    std::unique_ptr<Plugin> create(const PluginContext&,
                                   const plugin::Descriptor&) const override
    {
        return nullptr;
    }

    nlohmann::json buildExtensionPayload(
        const nlohmann::json& metadata) const override
    {
        return {
            {"Payload", metadata},
        };
    }
};

} // namespace

TEST(PluginRegistryTest, RegisterAndLookupFactory)
{
    PluginRegistry registry;

    registry.registerPlugin(testInterface, std::make_unique<TestFactory>());

    EXPECT_NE(registry.lookup(testInterface), nullptr);
}

TEST(PluginRegistryTest, LookupUnknownFactory)
{
    PluginRegistry registry;

    EXPECT_EQ(registry.lookup(testInterface), nullptr);
}

TEST(PluginRegistryTest, CreateDescriptor)
{
    PluginRegistry registry;

    registry.registerPlugin(testInterface, std::make_unique<TestFactory>());

    plugin::Request request{
        .interface = testInterface,
        .data = {},
    };

    auto descriptor = registry.createDescriptor(request);

    ASSERT_NE(descriptor, nullptr);

    EXPECT_EQ(descriptor->interface(), testInterface);
}

TEST(PluginRegistryTest, CreateDescriptorUnknownInterface)
{
    PluginRegistry registry;

    plugin::Request request{
        .interface = "unknown.interface",
        .data = {},
    };

    auto descriptor = registry.createDescriptor(request);

    EXPECT_EQ(descriptor, nullptr);
}

TEST(PluginRegistryTest, BuildExtensionPayloadUnknownInterface)
{
    PluginRegistry registry;

    nlohmann::json metadata{
        {"Key", "Value"},
    };

    auto payload =
        registry.buildExtensionPayload("unknown.interface", metadata);

    EXPECT_EQ(payload, metadata);
}

TEST(PluginRegistryTest, BuildExtensionPayloadUsesFactory)
{
    PluginRegistry registry;

    registry.registerPlugin(testInterface, std::make_unique<PayloadFactory>());

    nlohmann::json metadata{
        {"Key", "Value"},
    };

    auto payload = registry.buildExtensionPayload(testInterface, metadata);

    EXPECT_EQ(payload["Payload"]["Key"], "Value");
}

} // namespace phosphor::logging
