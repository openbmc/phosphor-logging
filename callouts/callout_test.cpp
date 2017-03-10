#include <iostream>
#include <sdbusplus/exception.hpp>
#include <phosphor-logging/elog.hpp>
#include <phosphor-logging/elog-errors.hpp>
#include "elog_meta.hpp"

using namespace phosphor::logging;
using namespace metadata;

int main(int argc, char** argv)
{
    if(2 != argc)
    {
        std::cerr << "usage: callout-test <sysfs path>" << std::endl;
        return -1;
    }

    try
    {
        using namespace example::xyz::openbmc_project::Example::Elog;
        elog<TestCallout>(
            TestCallout::DEV_ADDR(0xDEADEAD),
            TestCallout::CALLOUT_ERRNO_TEST(0),
            TestCallout::CALLOUT_DEVICE_PATH_TEST(argv[1]));
    }
    catch (TestCallout& e)
    {
        commit(e.name());
    }

    return 0;
}


