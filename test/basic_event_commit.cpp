#include <phosphor-logging/commit.hpp>
#include <xyz/openbmc_project/Logging/event.hpp>

#include <gtest/gtest.h>

namespace phosphor::logging::test
{

using LoggingCleared = sdbusplus::event::xyz::openbmc_project::Logging::Cleared;

TEST(TestBasicEventCommit, GenerateSimpleEvent)
{
    EXPECT_THROW(
        { throw LoggingCleared("NUMBER_OF_LOGS", 1); }, LoggingCleared);
    return;
}

TEST(TestBasicEventCommit, CallCommit)
{
    lg2::commit(LoggingCleared("NUMBER_OF_LOGS", 3));
}

} // namespace phosphor::logging::test
