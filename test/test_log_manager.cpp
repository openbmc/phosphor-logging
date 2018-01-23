#include "log_manager.hpp"

#include "xyz/openbmc_project/Common/error.hpp"
#include <phosphor-logging/elog-errors.hpp>

#include <gtest/gtest.h>
#include <sdbusplus/bus.hpp>

#include <stdlib.h>

#include <exception>
#include <experimental/filesystem>

#include <phosphor-logging/elog.hpp>
#include <phosphor-logging/log.hpp>
#include <phosphor-logging/elog-errors.hpp>

namespace phosphor
{
namespace logging
{
namespace internal
{

namespace fs = std::experimental::filesystem;

class TestLogManager : public testing::Test
{
    public:

        sdbusplus::bus::bus bus;
        Manager manager;
        TestLogManager()
            : bus(sdbusplus::bus::new_default()),
              manager(bus, "/xyz/openbmc_test/abc")
        {
            manager.setUnitTest(true);
        }

        ~TestLogManager()
        {

        }

        int getSize()
        {
            return manager.realErrors.size();
        }


};


TEST_F(TestLogManager, logEntries)
{
    for(auto i =0;i<10;i++)
    {
        manager.commit(i,"FOO");
    }
    EXPECT_EQ(10,getSize());
}

TEST_F(TestLogManager, logCap)
{
    for(auto i =0;i<210;i++)
    {
        manager.commit(i,"FOO");
    }
    // the log should be capped at 200 entries
    EXPECT_EQ(200,getSize());
}


}// nmaespace internal
}// namespce logging
}// namespace phosphor
