#include "config.h"

#include "log_manager.hpp"

#include <stdlib.h>

#include <sdbusplus/bus.hpp>
#include <sdbusplus/test/sdbus_mock.hpp>

#include <filesystem>
#include <stdexcept>

#include <gtest/gtest.h>

namespace phosphor
{
namespace logging
{
namespace test
{

namespace fs = std::filesystem;

sdbusplus::SdBusMock sdbusMock;
sdbusplus::bus_t bus = sdbusplus::get_mocked_new(&sdbusMock);

class TestSerialization : public testing::Test
{
  public:
    TestSerialization() : dir(createTempDir()), manager(bus, OBJ_INTERNAL) {}

    ~TestSerialization()
    {
        fs::remove_all(dir);
    }

    fs::path dir;
    phosphor::logging::internal::Manager manager;

  private:
    static fs::path createTempDir()
    {
        char tmplt[] = "/tmp/logging_test.XXXXXX";
        char* tmpDir = mkdtemp(tmplt);
        if (tmpDir == nullptr)
        {
            throw std::runtime_error("mkdtemp failed for serialization test");
        }
        return fs::path(tmpDir);
    }
};

} // namespace test
} // namespace logging
} // namespace phosphor
