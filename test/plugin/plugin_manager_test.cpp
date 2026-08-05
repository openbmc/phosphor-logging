#include "plugin/plugin_factory.hpp"
#include "plugin/plugin_manager.hpp"
#include "plugin/plugin_registry.hpp"

#include <gtest/gtest.h>

namespace phosphor::logging::test
{

namespace
{

class TestPlugin : public phosphor::logging::Plugin
{
  public:
    plugin::Type type() const override
    {
        return plugin::Type::cper;
    }
};

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

        return std::make_unique<TestPlugin>();
    }
};
} // namespace

class TestDescriptor : public phosphor::logging::plugin::Descriptor
{
  public:
    plugin::Type type() const override
    {
        return plugin::Type::cper;
    }
};

} // namespace

TEST(PluginManagerTest, CreatePlugin)
{
    PluginRegistry registry;

    registry.registerPlugin(plugin::Type::cper,
                            std::make_unique<TestFactory>());

    PluginManager manager(registry);

    auto bus = sdbusplus::bus::new_default();

    std::string objectPath = "/xyz/openbmc_project/logging/entry/1";

    PluginContext context{
        bus,
        objectPath,
    };

    TestDescriptor descriptor;

    auto plugin = manager.create(context, descriptor);

    ASSERT_NE(plugin, nullptr);
    EXPECT_EQ(plugin->type(), plugin::Type::cper);
}

TEST(PluginManagerTest, CreateUnknownPlugin)
{
    PluginRegistry registry;

    PluginManager manager(registry);

    auto bus = sdbusplus::bus::new_default();

    std::string objectPath = "/xyz/openbmc_project/logging/entry/1";

    PluginContext context{
        bus,
        objectPath,
    };

    TestDescriptor descriptor;

    EXPECT_EQ(manager.create(context, descriptor), nullptr);
}

TEST(PluginManagerTest, CreateMultiplePlugins)
{
    PluginRegistry registry;

    registry.registerPlugin(plugin::Type::cper,
                            std::make_unique<TestFactory>());

    PluginManager manager(registry);

    auto bus = sdbusplus::bus::new_default();

    std::string objectPath = "/xyz/openbmc_project/logging/entry/1";

    PluginContext context{
        bus,
        objectPath,
    };

    TestDescriptor descriptor1;
    TestDescriptor descriptor2;

    auto plugin1 = manager.create(context, descriptor1);

    auto plugin2 = manager.create(context, descriptor2);

    ASSERT_NE(plugin1, nullptr);
    ASSERT_NE(plugin2, nullptr);

    EXPECT_EQ(plugin1->type(), plugin::Type::cper);
    EXPECT_EQ(plugin2->type(), plugin::Type::cper);
}

} // namespace phosphor::logging::test
