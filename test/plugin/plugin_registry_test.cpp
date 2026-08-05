#include "plugin/plugin_factory.hpp"
#include "plugin/plugin_registry.hpp"

#include <gtest/gtest.h>

namespace phosphor::logging
{

namespace
{

class TestFactory : public PluginFactory
{
  public:
    PluginPtr create(const PluginContext&,
                     const plugin::Descriptor&) const override
    {
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

} // namespace phosphor::logging
