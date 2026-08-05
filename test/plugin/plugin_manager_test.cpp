#include "plugin/plugin_factory.hpp"
#include "plugin/plugin_manager.hpp"
#include "plugin/plugin_registry.hpp"

#include <gtest/gtest.h>

namespace phosphor::logging::test
{

namespace
{

constexpr auto testInterface = "xyz.openbmc_project.Logging.Test";

class TestPlugin : public phosphor::logging::Plugin
{
  public:
    std::string_view interface() const override
    {
        return testInterface;
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
    std::string_view interface() const override
    {
        return testInterface;
    }
};

} // namespace

TEST(PluginManagerTest, CreatePlugin)
{
    PluginRegistry registry;

    registry.registerPlugin(testInterface, std::make_unique<TestFactory>());

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
    EXPECT_EQ(plugin->interface(), testInterface);
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

    registry.registerPlugin(testInterface, std::make_unique<TestFactory>());

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

    EXPECT_EQ(plugin1->interface(), testInterface);
    EXPECT_EQ(plugin2->interface(), testInterface);
}

} // namespace phosphor::logging::test
