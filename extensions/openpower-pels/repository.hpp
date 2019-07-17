#pragma once
#include "bcd_time.hpp"
#include "pel.hpp"

#include <algorithm>
#include <filesystem>

namespace openpower
{
namespace pels
{

/**
 * @class Repository
 *
 * The class handles saving and retrieving PELs on the BMC.
 */
class Repository
{
  public:
    Repository() = delete;
    ~Repository() = default;
    Repository(const Repository&) = default;
    Repository& operator=(const Repository&) = default;
    Repository(Repository&&) = default;
    Repository& operator=(Repository&&) = default;

    /**
     * @brief Constructor
     *
     * @param[in] basePath - the base filesystem path for the repository
     */
    Repository(const std::filesystem::path& basePath);

    /**
     * @brief Adds a PEL to the repository
     *
     * Throws File.Error.Open or File.Error.Write exceptions on failure
     *
     * @param[in] pel - the PEL to add
     */
    void add(std::unique_ptr<PEL>& pel);

    /**
     * @brief Generates the filename to use for the PEL ID and BCDTime.
     *
     * @param[in] pelID - the PEL ID
     * @param[in] time - the BCD time
     *
     * @return string - A filename string of <BCD_time>_<pelID>
     */
    static std::string getPELFilename(uint32_t pelID, const BCDTime& time);

  private:
    /**
     * @brief The filesystem path to the PEL logs.
     */
    const std::filesystem::path _logPath;
};

} // namespace pels
} // namespace openpower
