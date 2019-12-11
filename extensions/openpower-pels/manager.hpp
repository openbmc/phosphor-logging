#pragma once

#include "data_interface.hpp"
#include "host_notifier.hpp"
#include "log_manager.hpp"
#include "paths.hpp"
#include "registry.hpp"
#include "repository.hpp"

namespace openpower
{
namespace pels
{

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
     * @param[in] dataIface - The data interface object
     */
    Manager(phosphor::logging::internal::Manager& logManager,
            std::unique_ptr<DataInterfaceBase> dataIface) :
        _logManager(logManager),
        _repo(getPELRepoPath()),
        _registry(getMessageRegistryPath() / message::registryFileName),
        _dataIface(std::move(dataIface))
    {
    }

    /**
     * @brief constructor that enables host notification
     *
     * @param[in] logManager - internal::Manager object
     * @param[in] dataIface - The data interface object
     * @param[in] hostIface - The hostInterface object
     */
    Manager(phosphor::logging::internal::Manager& logManager,
            std::unique_ptr<DataInterfaceBase> dataIface,
            std::unique_ptr<HostInterface> hostIface) :
        Manager(logManager, std::move(dataIface))
    {
        _hostNotifier = std::make_unique<HostNotifier>(
            _repo, *(_dataIface.get()), std::move(hostIface));
    }

    /**
     * @brief Creates a PEL based on the OpenBMC event log contents.  If
     *        a PEL was passed in via the RAWPEL specifier in the
     *        additionalData parameter, use that instead.
     *
     * @param[in] message - the event log message property
     * @param[in] obmcLogID - the corresponding OpenBMC event log id
     * @param[in] timestamp - the Timestamp property
     * @param[in] severity - the event log severity
     * @param[in] additionalData - the AdditionalData property
     * @param[in] associations - the Associations property
     */
    void create(const std::string& message, uint32_t obmcLogID,
                uint64_t timestamp, phosphor::logging::Entry::Level severity,
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
    bool isDeleteProhibited(uint32_t obmcLogID);

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
                   uint64_t timestamp, phosphor::logging::Entry::Level severity,
                   const std::vector<std::string>& additionalData,
                   const std::vector<std::string>& associations);

    /**
     * @brief Reference to phosphor-logging's Manager class
     */
    phosphor::logging::internal::Manager& _logManager;

    /**
     * @brief The PEL repository object
     */
    Repository _repo;

    /**
     * @brief The PEL message registry object
     */
    message::Registry _registry;

    /**
     * @brief The API the PEL sections use to gather data
     */
    std::unique_ptr<DataInterfaceBase> _dataIface;

    /**
     * @brief The HostNotifier object used for telling the
     *        host about new PELs
     */
    std::unique_ptr<HostNotifier> _hostNotifier;
};

} // namespace pels
} // namespace openpower
