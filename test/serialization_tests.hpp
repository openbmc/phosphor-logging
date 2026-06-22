#include "config.h"

#include "log_manager.hpp"

#include <stdlib.h>

#include <sdbusplus/bus.hpp>

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

class BusAccessor
{
  public:
    operator sdbusplus::bus_t&() const
    {
        static sdbusplus::bus_t bus = sdbusplus::bus::new_default();
        return bus;
    }
};

class ManagerAccessor
{
  public:
    operator phosphor::logging::internal::Manager&() const
    {
        static phosphor::logging::internal::Manager manager(
            static_cast<sdbusplus::bus_t&>(BusAccessor{}), OBJ_INTERNAL);
        return manager;
    }
};

inline const BusAccessor bus{};
inline const ManagerAccessor manager{};

class TestSerialization : public testing::Test
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
