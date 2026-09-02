#include "event_extensions/cper/raw.hpp"
#include "event_extensions/registry.hpp"

#include <gtest/gtest.h>

namespace phosphor::logging::event_extensions
{

TEST(CPERRawTest, RegisterProvider)
{
    Registry registry;
    cper::raw::registerProvider(registry);

    EXPECT_NE(registry.lookup(cper::raw::interface), nullptr);
}

} // namespace phosphor::logging::event_extensions
