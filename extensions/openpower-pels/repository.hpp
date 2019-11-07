#pragma once
#include "bcd_time.hpp"
#include "pel.hpp"

#include <algorithm>
#include <bitset>
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
     * @brief Structure of commonly used PEL attributes.
     */
    struct PELAttributes
    {
        std::filesystem::path path;
        std::bitset<16> actionFlags;

        PELAttributes() = delete;

        PELAttributes(const std::filesystem::path& path, uint16_t flags) :
            path(path), actionFlags(flags)
        {
        }
    };

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
        return findPEL(id) != _pelAttributes.end();
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

    using ForEachFunc = std::function<bool(const PEL&)>;

    /**
     * @brief Run a user defined function on every PEL in the repository.
     *
     * ForEachFunc takes a const PEL reference, and should return
     * true to stop iterating and return out of for_each.
     *
     * For example, to save up to 100 IDs in the repo into a vector:
     *
     *     std::vector<uint32_t> ids;
     *     ForEachFunc f = [&ids](const PEL& pel) {
     *         ids.push_back(pel.id());
     *         return ids.size() == 100 ? true : false;
     *     };
     *
     * @param[in] func - The function to run.
     */
    void for_each(ForEachFunc func) const;

    using AddCallback = std::function<void(const PEL&)>;

    /**
     * @brief Subscribe to PELs being added to the repository.
     *
     * Every time a PEL is added to the repository, the provided
     * function will be called with the new PEL as the argument.
     *
     * The function must be of type void(const PEL&).
     *
     * @param[in] name - The subscription name
     * @param[in] func - The callback function
     */
    void subscribeToAdds(const std::string& name, AddCallback func)
    {
        if (_addSubscriptions.find(name) == _addSubscriptions.end())
        {
            _addSubscriptions.emplace(name, func);
        }
    }

    /**
     * @brief Unsubscribe from new PELs.
     *
     * @param[in] name - The subscription name
     */
    void unsubscribeFromAdds(const std::string& name)
    {
        _addSubscriptions.erase(name);
    }

    using DeleteCallback = std::function<void(uint32_t)>;

    /**
     * @brief Subscribe to PELs being deleted from the repository.
     *
     * Every time a PEL is deleted from the repository, the provided
     * function will be called with the PEL ID as the argument.
     *
     * The function must be of type void(const uint32_t).
     *
     * @param[in] name - The subscription name
     * @param[in] func - The callback function
     */
    void subscribeToDeletes(const std::string& name, DeleteCallback func)
    {
        if (_deleteSubscriptions.find(name) == _deleteSubscriptions.end())
        {
            _deleteSubscriptions.emplace(name, func);
        }
    }

    /**
     * @brief Unsubscribe from deleted PELs.
     *
     * @param[in] name - The subscription name
     */
    void unsubscribeFromDeletes(const std::string& name)
    {
        _deleteSubscriptions.erase(name);
    }

    /**
     * @brief Get the PEL attributes for a PEL
     *
     * @param[in] id - The ID to find the attributes for
     *
     * @return The attributes or an empty optional if not found
     */
    std::optional<std::reference_wrapper<const PELAttributes>>
        getPELAttributes(const LogID& id) const;

  private:
    /**
     * @brief Finds an entry in the _pelAttributes map.
     *
     * @param[in] id - the ID (either the pel ID, OBMC ID, or both)
     *
     * @return an iterator to the entry
     */
    std::map<LogID, PELAttributes>::const_iterator
        findPEL(const LogID& id) const
    {
        return std::find_if(_pelAttributes.begin(), _pelAttributes.end(),
                            [&id](const auto& a) { return a.first == id; });
    }

    /**
     * @brief Call any subscribed functions for new PELs
     *
     * @param[in] pel - The new PEL
     */
    void processAddCallbacks(const PEL& pel) const;

    /**
     * @brief Call any subscribed functions for deleted PELs
     *
     * @param[in] id - The ID of the deleted PEL
     */
    void processDeleteCallbacks(uint32_t id) const;

    /**
     * @brief Restores the _pelAttributes map on startup based on the existing
     *        PEL data files.
     */
    void restore();

    /**
     * @brief Stores a PEL object in the filesystem.
     *
     * @param[in] pel - The PEL to write
     * @param[in] path - The file to write to
     *
     * Throws exceptions on failures.
     */
    void write(const PEL& pel, const std::filesystem::path& path);

    /**
     * @brief The filesystem path to the PEL logs.
     */
    const std::filesystem::path _logPath;

    /**
     * @brief A map of the PEL/OBMC IDs to PEL attributes.
     */
    std::map<LogID, PELAttributes> _pelAttributes;

    /**
     * @brief Subcriptions for new PELs.
     */
    std::map<std::string, AddCallback> _addSubscriptions;

    /**
     * @brief Subscriptions for deleted PELs.
     */
    std::map<std::string, DeleteCallback> _deleteSubscriptions;
};

} // namespace pels
} // namespace openpower
