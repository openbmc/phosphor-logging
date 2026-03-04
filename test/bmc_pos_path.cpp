#include "bmc_pos_path.hpp"

#include <cstring>
#include <fstream>

namespace phosphor::logging
{

// Use a function-local static to ensure initialization
static std::filesystem::path& getTestDir()
{
    static std::filesystem::path testDir;
    return testDir;
}

// Use a function-local static to ensure initialization
static std::string& getTestPath()
{
    static std::string testPath;
    return testPath;
}

void cleanupBMCPosTestDir()
{
    auto& testDir = getTestDir();
    auto& testPath = getTestPath();
    if (!testDir.empty())
    {
        std::error_code ec;
        std::filesystem::remove_all(testDir, ec);
        testDir.clear();
        testPath.clear();
    }
}

std::filesystem::path getBMCPositionFilePath()
{
    auto& testPath = getTestPath();

    if (!testPath.empty())
    {
        return testPath;
    }

    char templ[] = "/tmp/bmc_pos_test_XXXXXX";
    auto& testDir = getTestDir();
    testDir = mkdtemp(templ);
    if (testDir.empty())
    {
        throw std::runtime_error(
            std::string{"Unable to create temp directory: "} + strerror(errno));
    }
    testPath = testDir / "bmc_position";

    // Initialize the file with position 0
    std::ofstream file{testPath};
    if (!file)
    {
        throw std::runtime_error(
            std::string{"Unable to create BMC position file: "} + testPath);
    }
    size_t position = 0;
    file << position;

    return testPath;
}

} // namespace phosphor::logging
