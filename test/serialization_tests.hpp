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

char tmplt[] = "/tmp/logging_test.XXXXXX";

// Construct the D-Bus mock and the objects that depend on it per test
// instead of at namespace scope. Registering a gmock mock object accesses
// gmock's global mock object registry, and the C++ standard does not define
// the initialization order of globals in different translation units.
class MockedManagerTest : public testing::Test
{
  protected:
    sdbusplus::SdBusMock sdbusMock;
    sdbusplus::bus_t bus = sdbusplus::get_mocked_new(&sdbusMock);
    phosphor::logging::internal::Manager manager{bus, OBJ_INTERNAL};
};

class TestSerialization : public MockedManagerTest
{
  public:
    TestSerialization() : dir(fs::path(mkdtemp(tmplt))) {}

    ~TestSerialization()
    {
        fs::remove_all(dir);
    }

    fs::path dir;
};

} // namespace test
} // namespace logging
} // namespace phosphor
