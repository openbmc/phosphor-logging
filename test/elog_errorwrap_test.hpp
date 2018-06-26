#include "log_manager.hpp"
#include "xyz/openbmc_project/Common/error.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <sdbusplus/bus.hpp>
#include <experimental/filesystem>
#include <phosphor-logging/elog.hpp>
#include <phosphor-logging/log.hpp>
#include <phosphor-logging/elog-errors.hpp>
#include "elog_serialize.hpp"
#include "config.h"

namespace phosphor
{
namespace logging
{
namespace test
{

namespace fs = std::experimental::filesystem;

auto bus = sdbusplus::bus::new_default();
phosphor::logging::internal::Manager manager(bus, OBJ_INTERNAL);

class TestLogManager : public testing::Test
{
    public:
        TestLogManager()
        {
            std::experimental::filesystem::create_directories(ERRLOG_PERSIST_PATH);
        }

        ~TestLogManager()
        {
            fs::remove_all(ERRLOG_PERSIST_PATH);
        }

};


}// nmaespace test
}// namespace logging
}// namespace phosphor
