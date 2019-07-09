#pragma once

#include "elog_entry.hpp"
#include "log_manager.hpp"

namespace openpower
{
namespace pels
{

using namespace phosphor::logging;

/**
 * @brief PEL manager object
 */
class Manager
{
  public:
    Manager() = delete;
    ~Manager() = default;
    Manager(const Manager&) = default;
    Manager& operator=(const Manager&) = default;
    Manager(Manager&&) = default;
    Manager& operator=(Manager&&) = default;

    /**
     * @brief constructor
     *
     * @param[in] logManager - internal::Manager object
     */
    Manager(internal::Manager& logManager) : _logManager(logManager)
    {
    }

    /**
     * @brief Creates a PEL based on the OpenBMC event log contents.  If
     *        a PEL was passed in, use that instead.
     *
     * @param[in] message - the event log message property
     * @param[in] obmcLogID - the corresponding OpenBMC event log id
     * @param[in] timestamp - the Timestamp property
     * @param[in] severity - the event log severity
     * @param[in] additionalData - the AdditionalData property
     * @param[in] associations - the Associations property
     */
    void create(const std::string& message, uint32_t obmcLogID,
                uint64_t timestamp, Entry::Level severity,
                const std::vector<std::string>& additionalData,
                const std::vector<std::string>& associations);

    /**
     * @brief Erase a PEL based on its OpenBMC event log ID
     *
     * @param[in] obmcLogID - the corresponding OpenBMC event log id
     */
    void erase(uint32_t obmcLogID);

    /** @brief Says if an OpenBMC event log may not be manually deleted at this
     *         time because its corresponding PEL cannot be.
     *
     * There are PEL retention policies that can prohibit the manual deletion
     * of PELs (and therefore OpenBMC event logs).
     *
     * @param[in] obmcLogID - the OpenBMC event log ID
     * @return bool - true if prohibited
     */
    bool deleteProhibited(uint32_t obmcLogID);

  private:
    /**
     * @brief Adds a received raw PEL to the PEL repository
     *
     * @param[in] rawPelPath - The path to the file that contains the
     *                         raw PEL.
     * @param[in] obmcLogID - the corresponding OpenBMC event log id
     */
    void addRawPEL(const std::string& rawPelPath, uint32_t obmcLogID);

    /**
     * @brief Creates a PEL based on the OpenBMC event log contents.
     *
     * @param[in] message - The event log message property
     * @param[in] obmcLogID - the corresponding OpenBMC event log id
     * @param[in] timestamp - The timestamp property
     * @param[in] severity - The event log severity
     * @param[in] additionalData - The AdditionalData property
     * @param[in] associations - The associations property
     */
    void createPEL(const std::string& message, uint32_t obmcLogID,
                   uint64_t timestamp, Entry::Level severity,
                   const std::vector<std::string>& additionalData,
                   const std::vector<std::string>& associations);

    /**
     * @brief Reference to phosphor-logging's Manager class
     */
    internal::Manager& _logManager;
};

} // namespace pels
} // namespace openpower
