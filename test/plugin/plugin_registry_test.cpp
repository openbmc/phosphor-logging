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

} // namespace phosphor::logging
