#include "elog_errorwrap_test.hpp"

namespace phosphor
{
namespace logging
{
namespace test
{

TEST_F(TestLogManager, logCap)
{
    for(auto i =0; i<ERROR_INFO_CAP + 20; i++)
    {
        manager.commitWithLvl(i,"FOO", 6);
    }

    EXPECT_EQ(ERROR_INFO_CAP , manager.getInfoErrSize());

    for(auto i =0; i< ERROR_CAP + 20; i++)
    {
        manager.commitWithLvl(i,"FOO", 0);
    }
    // Max num of high severity errors can be created is qual to ERROR_CAP
    EXPECT_EQ(ERROR_CAP, manager.getRealErrSize());
    // Max num of Info( and below Sev) errors can be created is qual to
    // ERROR_INFO_CAP
    EXPECT_EQ(ERROR_INFO_CAP , manager.getInfoErrSize());
}

}// namespace test
}// namespace logging
}// namespace phosphor
