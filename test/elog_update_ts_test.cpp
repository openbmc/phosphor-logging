#include "config.h"

#include "elog_entry.hpp"
#include "elog_serialize.hpp"
#include "extensions.hpp"
#include "log_manager.hpp"
#include "paths.hpp"

#include <filesystem>
#include <thread>

#include <gtest/gtest.h>

namespace phosphor
{
namespace logging
{
namespace test
{

using namespace std::chrono_literals;
using Oem = phosphor::logging::OemType;
namespace fs = std::filesystem;

void deleteIsProhibitedMock(uint32_t /*id*/, bool& prohibited)
{
    prohibited = true;
}

void deleteIsNotProhibitedMock(uint32_t /*id*/, bool& prohibited)
{
    prohibited = false;
}

// Helper: generic multi-vendor OEM data
static Oem createTestOem()
{
    return {
        {"VendorA",
         {{"ErrorCode", uint64_t(42)}, {"Component", std::string("UnitA")}}},
        {"VendorB",
         {{"InstanceId", uint64_t(7)},
          {"DriverName", std::string("driver-x")}}},
        {"VendorC", {{"Flag", std::string("true")}, {"Count", uint64_t(100)}}}};
}

// Test that the update timestamp changes when the resolved property changes
TEST(TestUpdateTS, testChangeResolved)
{
    // Setting resolved will serialize, so need this directory.
    fs::create_directories(paths::error());

    if (!fs::exists(paths::error()))
    {
        ADD_FAILURE() << "Could not create " << paths::error() << "\n";
        exit(1);
    }

    auto bus = sdbusplus::bus::new_default();
    phosphor::logging::internal::Manager manager(bus, OBJ_INTERNAL);

    // Use a random number for the ID to avoid other CI
    // testcases running in parallel.
    std::srand(std::time(nullptr));
    uint32_t id = std::rand();

    if (fs::exists(fs::path{paths::error()} / std::to_string(id)))
    {
        std::cerr << "Another testcase is using ID " << id << "\n";
        id = std::rand();
    }

    uint64_t timestamp{100};
    std::string message{"test error"};
    std::string fwLevel{"level42"};
    std::string path{"/tmp/99"};
    std::map<std::string, std::string> testData{{"additional", "data"}};
    phosphor::logging::AssociationList associations{};

    Entry elog{bus,
               std::string(OBJ_ENTRY) + '/' + std::to_string(id),
               id,
               timestamp,
               Entry::Level::Informational,
               std::move(message),
               std::move(testData),
               std::move(associations),
               fwLevel,
               path,
               Oem{},
               manager};

    EXPECT_EQ(elog.timestamp(), elog.updateTimestamp());

    std::this_thread::sleep_for(1ms);

    elog.resolved(true);
    auto updateTS = elog.updateTimestamp();
    EXPECT_NE(updateTS, elog.timestamp());

    std::this_thread::sleep_for(1ms);

    elog.resolved(false);
    EXPECT_NE(updateTS, elog.updateTimestamp());
    updateTS = elog.updateTimestamp();

    std::this_thread::sleep_for(1ms);

    // No change
    elog.resolved(false);
    EXPECT_EQ(updateTS, elog.updateTimestamp());

    // Leave the directory in case other CI instances are running
    fs::remove(fs::path{paths::error()} / std::to_string(id));
}

TEST(TestResolveProhibited, testResolveFlagChange)
{
    auto persist_path = phosphor::logging::paths::error();

    // Setting resolved will serialize, so need this directory.
    fs::create_directories(persist_path);

    if (!fs::exists(persist_path))
    {
        ADD_FAILURE() << "Could not create "
                      << phosphor::logging::paths::error() << "\n";
        exit(1);
    }

    auto bus = sdbusplus::bus::new_default();
    phosphor::logging::internal::Manager manager(bus, OBJ_INTERNAL);

    // Use a random number for the ID to avoid other CI
    // testcases running in parallel.
    std::srand(std::time(nullptr));
    uint32_t id = std::rand();

    if (fs::exists(persist_path / std::to_string(id)))
    {
        std::cerr << "Another testcase is using ID " << id << "\n";
        id = std::rand();
    }

    uint64_t timestamp{100};
    std::string message{"test error"};
    std::string fwLevel{"level42"};
    std::string path{"/tmp/99"};
    std::map<std::string, std::string> testData{{"additional", "data"}};
    phosphor::logging::AssociationList associations{};

    Entry elog{bus,
               std::string(OBJ_ENTRY) + '/' + std::to_string(id),
               id,
               timestamp,
               Entry::Level::Informational,
               std::move(message),
               std::move(testData),
               std::move(associations),
               fwLevel,
               path,
               Oem{},
               manager};

    Extensions ext{deleteIsProhibitedMock};

    EXPECT_THROW(elog.resolved(true),
                 sdbusplus::xyz::openbmc_project::Common::Error::Unavailable);

    Extensions::getDeleteProhibitedFunctions().clear();

    Extensions e{deleteIsNotProhibitedMock};

    EXPECT_NO_THROW(elog.resolved(true));
    EXPECT_EQ(elog.resolved(), true);

    // Leave the directory in case other CI instances are running
    fs::remove(persist_path / std::to_string(id));
}

// Entry-level behavior test
TEST(ElogEntryTest, OemSetGet)
{
    auto bus = sdbusplus::bus::new_default();
    phosphor::logging::internal::Manager manager{bus, OBJ_INTERNAL};

    uint32_t id = 1;
    std::string path = std::string(OBJ_ENTRY) + "/" + std::to_string(id);

    Entry entry{bus, path, id, manager};

    auto oem = createTestOem();

    entry.oem(oem);

    EXPECT_EQ(entry.oem(), oem);
}

// Validate nested structure and variant correctness
TEST(ElogEntryTest, OemMultiVendorStructure)
{
    Oem oem = createTestOem();

    ASSERT_TRUE(oem.contains("VendorA"));
    ASSERT_TRUE(oem.contains("VendorB"));
    ASSERT_TRUE(oem.contains("VendorC"));

    EXPECT_EQ(std::get<uint64_t>(oem["VendorA"]["ErrorCode"]), 42);
    EXPECT_EQ(std::get<std::string>(oem["VendorA"]["Component"]), "UnitA");

    EXPECT_EQ(std::get<uint64_t>(oem["VendorB"]["InstanceId"]), 7);
    EXPECT_EQ(std::get<std::string>(oem["VendorB"]["DriverName"]), "driver-x");

    EXPECT_EQ(std::get<uint64_t>(oem["VendorC"]["Count"]), 100);
}

} // namespace test
} // namespace logging
} // namespace phosphor
