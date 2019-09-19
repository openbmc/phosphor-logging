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
    auto data = pelDataFactory(TestPelType::pelSimple);

    fs::path pelFilename = makeTempDir() / "rawpel";
    std::ofstream pelFile{pelFilename};
    pelFile.write(reinterpret_cast<const char*>(data->data()), data->size());
    pelFile.close();

    std::string adItem = "RAWPEL=" + pelFilename.string();
    std::vector<std::string> additionalData{adItem};
    std::vector<std::string> associations;

    manager.create("error message", 42, 0,
                   phosphor::logging::Entry::Level::Error, additionalData,
                   associations);

    // We don't know the exact name, but a file should have been added to the
    // repo of the form <timestamp>_<ID>
    std::regex expr{"\\d+_\\d+"};

    bool found = false;
    for (auto& f : fs::directory_iterator(getPELRepoPath() / "logs"))
    {
        if (std::regex_search(f.path().string(), expr))
        {
            found = true;
            break;
        }
    }

    EXPECT_TRUE(found);

    // Now remove it based on its OpenBMC event log ID
    manager.erase(42);

    found = false;

    for (auto& f : fs::directory_iterator(getPELRepoPath() / "logs"))
    {
        if (std::regex_search(f.path().string(), expr))
        {
            found = true;
            break;
        }
    }

    EXPECT_FALSE(found);

    fs::remove_all(pelFilename.parent_path());
}
