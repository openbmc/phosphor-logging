#include "config.h"

#include "log_manager.hpp"

#include <stdlib.h>

#include <sdbusplus/bus.hpp>
#include <sdbusplus/test/sdbus_mock.hpp>

#include <filesystem>

#include <gtest/gtest.h>

namespace phosphor
{
namespace logging
{
namespace test
{

namespace fs = std::filesystem;

class SerializationTestBase : public testing::Test
{
  public:
    SerializationTestBase() :
        bus(sdbusplus::get_mocked_new(&sdbusMock)), manager(bus, OBJ_INTERNAL)
    {}

    sdbusplus::SdBusMock sdbusMock;
    sdbusplus::bus_t bus;
    phosphor::logging::internal::Manager manager;
};

class TestSerialization : public SerializationTestBase
{
  public:
    TestSerialization()
    {
        char tmplt[] = "/tmp/logging_test.XXXXXX";
        if (auto* p = mkdtemp(tmplt); p != nullptr)
        {
            dir = fs::path(p);
        }
    }

    ~TestSerialization()
    {
        if (!dir.empty())
        {
            fs::remove_all(dir);
        }
    }

    fs::path dir;
};

} // namespace test
} // namespace logging
} // namespace phosphor
