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

class TestFactory : public phosphor::logging::PluginFactory
{
  public:
    std::unique_ptr<phosphor::logging::Plugin> create(
        const PluginContext&, const plugin::Descriptor&) const override
    {
        return std::make_unique<TestPlugin>();
    }
};

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

} // namespace phosphor::logging::test
