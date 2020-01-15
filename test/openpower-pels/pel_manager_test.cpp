/**
 * Copyright © 2019 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "extensions/openpower-pels/manager.hpp"
#include "log_manager.hpp"
#include "pel_utils.hpp"

#include <fstream>
#include <regex>

#include <gtest/gtest.h>

using namespace openpower::pels;
namespace fs = std::filesystem;

class ManagerTest : public CleanPELFiles
{
};

fs::path makeTempDir()
{
    char path[] = "/tmp/tempnameXXXXXX";
    std::filesystem::path dir = mkdtemp(path);
    return dir;
}

std::optional<fs::path> findAnyPELInRepo()
{
    // PELs are named <timestamp>_<ID>
    std::regex expr{"\\d+_\\d+"};

    for (auto& f : fs::directory_iterator(getPELRepoPath() / "logs"))
    {
        if (std::regex_search(f.path().string(), expr))
        {
            return f.path();
        }
    }
    return std::nullopt;
}

// Test that using the RAWPEL=<file> with the Manager::create() call gets
// a PEL saved in the repository.
TEST_F(ManagerTest, TestCreateWithPEL)
{
    auto bus = sdbusplus::bus::new_default();
    phosphor::logging::internal::Manager logManager(bus, "logging_path");
    std::unique_ptr<DataInterfaceBase> dataIface =
        std::make_unique<DataInterface>(bus);

    openpower::pels::Manager manager{logManager, std::move(dataIface)};

    // Create a PEL, write it to a file, and pass that filename into
    // the create function.
    auto data = pelDataFactory(TestPELType::pelSimple);

    fs::path pelFilename = makeTempDir() / "rawpel";
    std::ofstream pelFile{pelFilename};
    pelFile.write(reinterpret_cast<const char*>(data.data()), data.size());
    pelFile.close();

    std::string adItem = "RAWPEL=" + pelFilename.string();
    std::vector<std::string> additionalData{adItem};
    std::vector<std::string> associations;

    manager.create("error message", 42, 0,
                   phosphor::logging::Entry::Level::Error, additionalData,
                   associations);

    // Find the file in the PEL repository directory
    auto pelPathInRepo = findAnyPELInRepo();

    EXPECT_TRUE(pelPathInRepo);

    // Now remove it based on its OpenBMC event log ID
    manager.erase(42);

    pelPathInRepo = findAnyPELInRepo();

    EXPECT_FALSE(pelPathInRepo);

    fs::remove_all(pelFilename.parent_path());
}

// Test that the message registry can be used to build a PEL.
TEST_F(ManagerTest, TestCreateWithMessageRegistry)
{
    const auto registry = R"(
{
    "PELs":
    [
        {
            "Name": "xyz.openbmc_project.Error.Test",
            "Subsystem": "power_supply",
            "ActionFlags": ["service_action", "report"],
            "SRC":
            {
                "ReasonCode": "0x2030"
            },
            "Documentation":
            {
                "Description": "A PGOOD Fault",
                "Message": "PS had a PGOOD Fault"
            }
        }
    ]
}
)";

    auto path = getMessageRegistryPath();
    fs::create_directories(path);
    path /= "message_registry.json";

    std::ofstream registryFile{path};
    registryFile << registry;
    registryFile.close();

    auto bus = sdbusplus::bus::new_default();
    phosphor::logging::internal::Manager logManager(bus, "logging_path");

    std::unique_ptr<DataInterfaceBase> dataIface =
        std::make_unique<DataInterface>(logManager.getBus());

    openpower::pels::Manager manager{logManager, std::move(dataIface)};

    std::vector<std::string> additionalData;
    std::vector<std::string> associations;

    // Create the event log to create the PEL from.
    manager.create("xyz.openbmc_project.Error.Test", 33, 0,
                   phosphor::logging::Entry::Level::Error, additionalData,
                   associations);

    // Ensure a PEL was created in the repository
    auto pelFile = findAnyPELInRepo();
    ASSERT_TRUE(pelFile);

    auto data = readPELFile(*pelFile);
    PEL pel(*data);

    // Spot check it.  Other testcases cover the details.
    EXPECT_TRUE(pel.valid());
    EXPECT_EQ(pel.obmcLogID(), 33);
    EXPECT_EQ(pel.primarySRC().value()->asciiString(),
              "BD612030                        ");

    // Remove it
    manager.erase(33);
    pelFile = findAnyPELInRepo();
    EXPECT_FALSE(pelFile);

    // Create an event log that can't be found in the registry.
    manager.create("xyz.openbmc_project.Error.Foo", 33, 0,
                   phosphor::logging::Entry::Level::Error, additionalData,
                   associations);

    // Currently, no PEL should be created.  Eventually, a 'missing registry
    // entry' PEL will be there.
    pelFile = findAnyPELInRepo();
    EXPECT_FALSE(pelFile);
}
