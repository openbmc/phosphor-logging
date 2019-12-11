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
    void SetUp() override
    {
        pelIDFile = openpower::pels::getPELIDFile();
        repoPath = openpower::pels::getPELRepoPath();
        registryPath = openpower::pels::getMessageRegistryPath();
    }

    void TearDown() override
    {
        std::filesystem::remove_all(
            std::filesystem::path{pelIDFile}.parent_path());
        std::filesystem::remove_all(repoPath);
        std::filesystem::remove_all(registryPath);
    }

    static std::filesystem::path pelIDFile;
    static std::filesystem::path repoPath;
    static std::filesystem::path registryPath;
};

/**
 * @brief Tells the factory which PEL to create
 */
enum class TestPELType
{
    pelSimple,
    privateHeaderSection,
    userHeaderSection,
    primarySRCSection,
    primarySRCSection2Callouts,
    failingMTMSSection
};

/**
 * @brief Tells the SRC factory which data to create
 */
enum class TestSRCType
{
    fruIdentityStructure,
    pceIdentityStructure,
    mruStructure,
    calloutStructureA,
    calloutStructureB,
    calloutSection2Callouts
};

/**
 * @brief PEL data factory, for testing
 *
 * @param[in] type - the type of data to create
 *
 * @return std::vector<uint8_t> - the PEL data
 */
std::vector<uint8_t> pelDataFactory(TestPELType type);

/**
 * @brief SRC data factory, for testing
 *
 * Provides pieces of the SRC PEL section, such as a callout.
 *
 * @param[in] type - the type of data to create
 *
 * @return std::vector<uint8_t> - The SRC data
 */
std::vector<uint8_t> srcDataFactory(TestSRCType type);

/**
 * @brief Helper function to read raw PEL data from a file
 *
 * @param[in] path - the path to read
 *
 * @return std::unique_ptr<std::vector<uint8_t>> - the data from the file
 */
std::unique_ptr<std::vector<uint8_t>>
    readPELFile(const std::filesystem::path& path);
