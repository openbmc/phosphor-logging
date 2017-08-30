#include <gtest/gtest.h>
#include <experimental/filesystem>
#include <stdlib.h>
#include <sdbusplus/bus.hpp>
#include "log_manager.hpp"
#include "config.h"

namespace phosphor
{
namespace logging
{
namespace test
{

namespace fs = std::experimental::filesystem;

char tmplt[] = "/tmp/logging_test.XXXXXX";
auto bus = sdbusplus::bus::new_default();
phosphor::logging::internel::Manager manager(bus, OBJ_INTERNAL);

class TestSerialization : public testing::Test
{
    public:
        TestSerialization() :
            dir(fs::path(mkdtemp(tmplt)))
        {
        }

        ~TestSerialization()
        {
            fs::remove_all(dir);
        }

        fs::path dir;
};

} // namespace test
} // namespace logging
} // namespace phosphor


