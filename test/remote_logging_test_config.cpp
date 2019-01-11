#include "remote_logging_tests.hpp"

#include <fstream>
#include <string>

#if __has_include(<filesystem>)
#include <filesystem>
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
namespace std
{
// splice experimental::filesystem into std
namespace filesystem = std::experimental::filesystem;
} // namespace std
#else
#error filesystem not available
#endif

namespace phosphor
{
namespace logging
{
namespace test
{

std::string getConfig(const char* filePath)
{
    std::fstream stream(filePath, std::fstream::in);
    std::string line;
    std::getline(stream, line);
    return line;
}

TEST_F(TestRemoteLogging, testOnlyAddress)
{
    config->address("1.1.1.1");
    EXPECT_EQ(fs::exists(configFilePath.c_str()), false);
}

TEST_F(TestRemoteLogging, testOnlyPort)
{
    config->port(100);
    EXPECT_EQ(fs::exists(configFilePath.c_str()), false);
}

TEST_F(TestRemoteLogging, testGoodConfig)
{
    config->address("1.1.1.1");
    config->port(100);
    EXPECT_EQ(getConfig(configFilePath.c_str()), "*.* @@1.1.1.1:100");
}

TEST_F(TestRemoteLogging, testClearAddress)
{
    config->address("1.1.1.1");
    config->port(100);
    EXPECT_EQ(getConfig(configFilePath.c_str()), "*.* @@1.1.1.1:100");
    config->address("");
    EXPECT_EQ(fs::exists(configFilePath.c_str()), false);
}

TEST_F(TestRemoteLogging, testClearPort)
{
    config->address("1.1.1.1");
    config->port(100);
    EXPECT_EQ(getConfig(configFilePath.c_str()), "*.* @@1.1.1.1:100");
    config->port(0);
    EXPECT_EQ(fs::exists(configFilePath.c_str()), false);
}

} // namespace test
} // namespace logging
} // namespace phosphor
