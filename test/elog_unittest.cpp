#include "elog_unittest.hpp"


namespace phosphor
{
namespace logging
{
namespace internal
{


TEST_F(TestLogManager, logEntries)
{
    for(auto i =0;i< ERROR_INFO_CAP;i++)
    {
        manager.commitWithLvl(i,"FOO", 4);
    }
    EXPECT_EQ(ERROR_INFO_CAP, getSize());
}



TEST_F(TestLogManager, logCap)
{
    for(auto i =0;i<ERROR_CAP + 20;i++)
    {
        manager.commitWithLvl(i,"FOO", 3);
    }
    // the log should be capped at ERROR_CAP entries
    EXPECT_EQ(ERROR_CAP,getSize());
}




}// nmaespace internal
}// namespce logging
}// namespace phosphor
