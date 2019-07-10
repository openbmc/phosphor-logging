#include <filesystem>
#include <memory>
#include <vector>

#include <gtest/gtest.h>

/**
 * @brief Tells the factory which PEL to create
 */
enum class TestPelType
{
    pelSimple,
    privateHeaderSimple
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
