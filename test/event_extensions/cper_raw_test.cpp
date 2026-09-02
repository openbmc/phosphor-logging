#include "event_extensions/cper/raw.hpp"
#include "event_extensions/registry.hpp"

#include <gtest/gtest.h>

namespace phosphor::logging::event_extensions
{

TEST(CPERRawTest, RegisterExtension)
{
    Registry registry;
    cper::raw::registerExtension(registry);
    const auto* registration = registry.find(cper::raw::interface);
    ASSERT_NE(registration, nullptr);

    EXPECT_TRUE(static_cast<bool>(registration->createCallback));
    EXPECT_TRUE(static_cast<bool>(registration->restoreCallback));
}

TEST(CPERRawTest, InterfaceName)
{
    EXPECT_EQ(cper::raw::interface,
              "xyz.openbmc_project.Logging.Extension.CPER.Raw");
}

TEST(CPERRawTest, CreateAndRestoreCallbacksRegistered)
{
    Registry registry;
    cper::raw::registerExtension(registry);
    const auto* registration = registry.find(cper::raw::interface);
    ASSERT_NE(registration, nullptr);

    EXPECT_TRUE(static_cast<bool>(registration->createCallback));
    EXPECT_TRUE(static_cast<bool>(registration->restoreCallback));
}

} // namespace phosphor::logging::event_extensions
