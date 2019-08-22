#pragma once
#include "bcd_time.hpp"
#include "pel.hpp"

#include <algorithm>
#include <filesystem>
#include <map>

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
    /**
     * @brief A structure that holds both the PEL and corresponding
     *        OpenBMC IDs.
     * Used for correlating the IDs with their data files for quick
     * lookup.  To find a PEL based on just one of the IDs, just use
     * the constructor that takes that ID.
     */
    struct LogID
    {
        struct Pel
        {
            uint32_t id;
            explicit Pel(uint32_t i) : id(i)
            {
            }
        };
        struct Obmc
        {
            uint32_t id;
            explicit Obmc(uint32_t i) : id(i)
            {
            }
        };

        Pel pelID;

        Obmc obmcID;

        LogID(Pel pel, Obmc obmc) : pelID(pel), obmcID(obmc)
        {
        }

        explicit LogID(Pel id) : pelID(id), obmcID(0)
        {
        }

        explicit LogID(Obmc id) : pelID(0), obmcID(id)
        {
        }

        LogID() = delete;

        /**
         * @brief A == operator that will match on either ID
         *        being equal if the other is zero, so that
         *        one can look up a PEL with just one of the IDs.
         */
        bool operator==(const LogID& id) const
        {
            if (id.pelID.id != 0)
            {
                return id.pelID.id == pelID.id;
            }
            if (id.obmcID.id != 0)
            {
                return id.obmcID.id == obmcID.id;
            }
            return false;
        }

        bool operator<(const LogID& id) const
        {
            return pelID.id < id.pelID.id;
        }
    };

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
     * @brief Removes a PEL from the repository
     *
     * @param[in] id - the ID (either the pel ID, OBMC ID, or both) to remove
     */
    void remove(const LogID& id);

    /**
     * @brief Generates the filename to use for the PEL ID and BCDTime.
     *
     * @param[in] pelID - the PEL ID
     * @param[in] time - the BCD time
     *
     * @return string - A filename string of <BCD_time>_<pelID>
     */
    static std::string getPELFilename(uint32_t pelID, const BCDTime& time);

    /**
     * @brief Returns true if the PEL with the specified ID is in the repo.
     *
     * @param[in] id - the ID (either the pel ID, OBMC ID, or both)
     * @return bool - true if that PEL is present
     */
    inline bool hasPEL(const LogID& id)
    {
        return findPEL(id) != _idsToPELs.end();
    }

    /**
     * @brief helper function for PEL tool.
     *
     * @return cosnt  std::map<LogID, std::filesystem::path>&
     */
    const std::map<LogID, std::filesystem::path>& getPELMap()
    {
        return this->_idsToPELs;
    }

    /**
     * @brief Returns the PEL data based on its ID.
     *
     * If the data can't be found for that ID, then the optional object
     * will be empty.
     *
     * @param[in] id - the LogID to get the PEL for, which can be either a
     *                 PEL ID or OpenBMC log ID.
     * @return std::optional<std::vector<uint8_t>> - the PEL data
     */
    std::optional<std::vector<uint8_t>> getPELData(const LogID& id);

  private:
    /**
     * @brief Finds an entry in the _idsToPELs map.
     *
     * @param[in] id - the ID (either the pel ID, OBMC ID, or both)
     *
     * @return an iterator to the entry
     */
    std::map<LogID, std::filesystem::path>::iterator findPEL(const LogID& id)
    {
        return std::find_if(_idsToPELs.begin(), _idsToPELs.end(),
                            [&id](const auto& i) { return i.first == id; });
    }

    /**
     * @brief Restores the _idsToPELs map on startup based on the existing
     *        PEL data files.
     */
    void restore();

    /**
     * @brief The filesystem path to the PEL logs.
     */
    const std::filesystem::path _logPath;

    /**
     * @brief A map of the PEL/OBMC IDs to the PEL data files.
     */
    std::map<LogID, std::filesystem::path> _idsToPELs;
};

} // namespace pels
} // namespace openpower
