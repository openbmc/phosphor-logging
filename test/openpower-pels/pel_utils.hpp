#include "extensions/openpower-pels/paths.hpp"

#include <filesystem>
#include <memory>
#include <vector>

#include <gtest/gtest.h>

/**
 * @brief Test fixture to remove the pelID file that PELs use.
 */
class CleanLogID : public ::testing::Test
{
  protected:
    static void SetUpTestCase()
    {
        pelIDFile = openpower::pels::getPELIDFile();
    }

    static void TearDownTestCase()
    {
        std::filesystem::remove_all(
            std::filesystem::path{pelIDFile}.parent_path());
    }

    static std::filesystem::path pelIDFile;
};

class CleanPELFiles : public ::testing::Test
{
  protected:
    static void SetUpTestCase()
    {
        pelIDFile = openpower::pels::getPELIDFile();
        repoPath = openpower::pels::getPELRepoPath();
    }

    static void TearDownTestCase()
    {
        std::filesystem::remove_all(
            std::filesystem::path{pelIDFile}.parent_path());
        std::filesystem::remove_all(repoPath);
    }

    static std::filesystem::path pelIDFile;
    static std::filesystem::path repoPath;
};

/**
 * @brief Tells the factory which PEL to create
 */
enum class TestPelType
{
    pelSimple,
    privateHeaderSimple,
    userHeaderSimple
};

/**
 * @brief PEL data factory, for testing
 *
 * @param[in] type - the type of data to create
 *
 * @return std::unique_ptr<std::vector<uint8_t>> - the PEL data
 */
std::unique_ptr<std::vector<uint8_t>> pelDataFactory(TestPelType type);

/**
 * @brief Helper function to read raw PEL data from a file
 *
 * @param[in] path - the path to read
 *
 * @return std::unique_ptr<std::vector<uint8_t>> - the data from the file
 */
std::unique_ptr<std::vector<uint8_t>>
    readPELFile(const std::filesystem::path& path);
