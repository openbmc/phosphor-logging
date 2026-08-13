#include "plugin/plugin_factory.hpp"
#include "plugin/plugin_registry.hpp"
#include "plugin/plugin_type.hpp"

#include <gtest/gtest.h>

namespace phosphor::logging
{

namespace
{

constexpr auto testInterface = "xyz.openbmc_project.Logging.Test";

class TestFactory : public PluginFactory
{
  public:
    std::unique_ptr<plugin::Descriptor> createDescriptor(
        const plugin::Info& info) const override
    {
        (void)info;
        return nullptr;
    }

    std::unique_ptr<Plugin> create(
        const PluginContext& context,
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
        const plugin::Info&) const override
    {
        return nullptr;
    }

    std::unique_ptr<Plugin> create(const PluginContext&,
                                   const plugin::Descriptor&) const override
    {
        return nullptr;
    }

    nlohmann::json buildPayload(const nlohmann::json& metadata) const override
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

    plugin::Info info{
        .interface = testInterface,
        .data = {},
    };

    auto descriptor = registry.createDescriptor(info);

    EXPECT_EQ(descriptor, nullptr);
}

TEST(PluginRegistryTest, CreateDescriptorUnknownInterface)
{
    PluginRegistry registry;

    plugin::Info info{
        .interface = "unknown.interface",
        .data = {},
    };

    auto descriptor = registry.createDescriptor(info);

    EXPECT_EQ(descriptor, nullptr);
}

TEST(PluginRegistryTest, BuildPayloadUnknownInterface)
{
    PluginRegistry registry;

    nlohmann::json metadata{
        {"Key", "Value"},
    };

    auto payload = registry.buildPayload("unknown.interface", metadata);

    EXPECT_EQ(payload, metadata);
}

TEST(PluginRegistryTest, BuildPayloadUsesFactory)
{
    PluginRegistry registry;

    registry.registerPlugin(testInterface, std::make_unique<PayloadFactory>());

    nlohmann::json metadata{
        {"Key", "Value"},
    };

    auto payload = registry.buildPayload(testInterface, metadata);

    EXPECT_EQ(payload["Payload"]["Key"], "Value");
}

} // namespace phosphor::logging
