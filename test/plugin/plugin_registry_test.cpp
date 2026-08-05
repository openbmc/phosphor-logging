#include "plugin/plugin_factory.hpp"
#include "plugin/plugin_registry.hpp"
#include "plugin/plugin_type.hpp"

#include <gtest/gtest.h>

namespace phosphor::logging
{

namespace
{

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

    registry.registerPlugin(plugin::Type::cper,
                            std::make_unique<TestFactory>());

    EXPECT_NE(registry.lookup(plugin::Type::cper), nullptr);
}

TEST(PluginRegistryTest, LookupUnknownFactory)
{
    PluginRegistry registry;

    EXPECT_EQ(registry.lookup(plugin::Type::cper), nullptr);
}

TEST(PluginRegistryTest, CreateDescriptor)
{
    PluginRegistry registry;

    registry.registerPlugin(plugin::Type::cper,
                            std::make_unique<TestFactory>());

    plugin::Info info{
        .type = plugin::Type::cper,
        .data = {},
    };

    auto descriptor = registry.createDescriptor(plugin::Type::cper, info);

    EXPECT_EQ(descriptor, nullptr);
}

} // namespace phosphor::logging
