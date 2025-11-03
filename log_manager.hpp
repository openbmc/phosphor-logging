#pragma once

#include "config.h"

#include "bmc_pos_mgr.hpp"
#include "elog_block.hpp"
#include "elog_entry.hpp"
#include "xyz/openbmc_project/Logging/Internal/Manager/server.hpp"

#include <phosphor-logging/log.hpp>
#include <sdbusplus/bus.hpp>
#include <xyz/openbmc_project/Collection/DeleteAll/server.hpp>
#include <xyz/openbmc_project/Logging/Create/server.hpp>
#include <xyz/openbmc_project/Logging/Entry/server.hpp>
#include <xyz/openbmc_project/Logging/event.hpp>

#include <list>

namespace phosphor
{
namespace logging
{

extern const std::map<std::string, std::vector<std::string>> g_errMetaMap;
extern const std::map<std::string, level> g_errLevelMap;

using CreateIface = sdbusplus::server::xyz::openbmc_project::logging::Create;
using DeleteAllIface =
    sdbusplus::server::xyz::openbmc_project::collection::DeleteAll;

using Severity = sdbusplus::xyz::openbmc_project::Logging::server::Entry::Level;
using LoggingCleared = sdbusplus::event::xyz::openbmc_project::Logging::Cleared;

namespace details
{
template <typename... T>
using ServerObject = typename sdbusplus::server::object_t<T...>;

using ManagerIface =
    sdbusplus::server::xyz::openbmc_project::logging::internal::Manager;

} // namespace details

constexpr size_t ffdcFormatPos = 0;
constexpr size_t ffdcSubtypePos = 1;
constexpr size_t ffdcVersionPos = 2;
constexpr size_t ffdcFDPos = 3;

using FFDCEntry = std::tuple<CreateIface::FFDCFormat, uint8_t, uint8_t,
                             sdbusplus::message::unix_fd>;

using FFDCEntries = std::vector<FFDCEntry>;

namespace internal
{

/** @class Manager
 *  @brief OpenBMC logging manager implementation.
 *  @details A concrete implementation for the
 *  xyz.openbmc_project.Logging.Internal.Manager DBus API.
 */
class Manager : public details::ServerObject<details::ManagerIface>
{
  public:
    Manager() = delete;
    Manager(const Manager&) = delete;
    Manager& operator=(const Manager&) = delete;
    Manager(Manager&&) = delete;
    Manager& operator=(Manager&&) = delete;
    virtual ~Manager() = default;

    /** @brief Constructor to put object onto bus at a dbus path.
     *  @param[in] bus - Bus to attach to.
     *  @param[in] path - Path to attach at.
     */
    Manager(sdbusplus::bus_t& bus, const char* objPath) :
        details::ServerObject<details::ManagerIface>(bus, objPath), busLog(bus),
        entryId(0), fwVersion(readFWVersion())
    {
        if constexpr (USE_BMC_POS_IN_ID)
        {
            bmcPosMgr = std::make_unique<BMCPosMgr>(bus);
        }
    };

    /*
     * @fn commit()
     * @brief sd_bus Commit method implementation callback.
     * @details Create an error/event log based on transaction id and
     *          error message.
     * @param[in] transactionId - Unique identifier of the journal entries
     *                            to be committed.
     * @param[in] errMsg - The error exception message associated with the
     *                     error log to be committed.
     */
    uint32_t commit(uint64_t transactionId, std::string errMsg) override;

    /*
     * @fn commit()
     * @brief sd_bus CommitWithLvl method implementation callback.
     * @details Create an error/event log based on transaction id and
     *          error message.
     * @param[in] transactionId - Unique identifier of the journal entries
     *                            to be committed.
     * @param[in] errMsg - The error exception message associated with the
     *                     error log to be committed.
     * @param[in] errLvl - level of the error
     */
    uint32_t commitWithLvl(uint64_t transactionId, std::string errMsg,
                           uint32_t errLvl) override;

    /** @brief Erase specified entry d-bus object
     *
     * @param[in] entryId - unique identifier of the entry
     */
    void erase(uint32_t entryId);

    /** @brief Construct error d-bus objects from their persisted
     *         representations.
     */
    void restore();

    /** @brief  Erase all error log entries
     *
     *  @return size_t - count of erased entries
     */
    size_t eraseAll();

    /** @brief Returns the count of high severity errors
     *
     *  @return int - count of real errors
     */
    int getRealErrSize();

    /** @brief Returns the count of Info errors
     *
     *  @return int - count of info errors
     */
    int getInfoErrSize();

    /** @brief Returns the number of blocking errors
     *
     *  @return int - count of blocking errors
     */
    int getBlockingErrSize()
    {
        return blockingErrors.size();
    }

    /** @brief Returns the number of property change callback objects
     *
     *  @return int - count of property callback entries
     */
    int getEntryCallbackSize()
    {
        return propChangedEntryCallback.size();
    }

    /**
     * @brief Returns the sdbusplus bus object
     *
     * @return sdbusplus::bus_t&
     */
    sdbusplus::bus_t& getBus()
    {
        return busLog;
    }

    /**
     * @brief Returns the ID of the last created entry
     *
     * @return uint32_t - The ID
     */
    uint32_t lastEntryID() const
    {
        return entryId;
    }

    /** @brief Creates an event log
     *
     *  This is an alternative to the _commit() API.  It doesn't use
     *  the journal to look up event log metadata like _commit does.
     *
     * @param[in] errMsg - The error exception message associated with the
     *                     error log to be committed.
     * @param[in] severity - level of the error
     * @param[in] additionalData - The AdditionalData property for the error
     * @param[in] ffdc - A vector of tuples that allows one to pass in file
     *                   descriptors for files that contain FFDC (First
     *                   Failure Data Capture). These will be passed to any
     *                   event logging extensions.
     */
    auto create(const std::string& message, Severity severity,
                const std::map<std::string, std::string>& additionalData,
                const FFDCEntries& ffdc = FFDCEntries{})
        -> sdbusplus::message::object_path;

    /** @brief Create an internal event log from the sdbusplus generated event
     *
     *  @param[in] event - The event to create.
     */
    auto createFromEvent(sdbusplus::exception::generated_event_base&& event)
        -> sdbusplus::message::object_path;

    /** @brief Common wrapper for creating an Entry object
     *
     * @return true if quiesce on error setting is enabled, false otherwise
     */
    bool isQuiesceOnErrorEnabled();

    /** @brief Create boot block association and quiesce host if running
     *
     * @param[in] entryId - The ID of the phosphor logging error
     */
    void quiesceOnError(const uint32_t entryId);

    /** @brief Check if inventory callout present in input entry
     *
     * @param[in] entry - The error to check for callouts
     *
     * @return true if inventory item in associations, false otherwise
     */
    bool isCalloutPresent(const Entry& entry);

    /** @brief Check (and remove) entry being erased from blocking errors
     *
     * @param[in] entryId - The entry that is being erased
     */
    void checkAndRemoveBlockingError(uint32_t entryId);

    /**
     * @brief Removes entries with 'no BMC position' in their IDs.
     *
     * Only does anything if the use-bmc-pos-in-id option is enabled.
     */
    void removeLogsWithNoPosition();

    /** @brief Persistent map of Entry dbus objects and their ID */
    std::map<uint32_t, std::unique_ptr<Entry>> entries;

  private:
    /*
     * @fn _commit()
     * @brief commit() helper
     * @param[in] transactionId - Unique identifier of the journal entries
     *                            to be committed.
     * @param[in] errMsg - The error exception message associated with the
     *                     error log to be committed.
     * @param[in] errLvl - level of the error
     */
    void _commit(uint64_t transactionId, std::string&& errMsg,
                 Entry::Level errLvl);

    /** @brief Call metadata handler(s), if any. Handlers may create
     *         associations.
     *  @param[in] errorName - name of the error
     *  @param[in] additionalData - list of metadata (in key=value format)
     *  @param[out] objects - list of error's association objects
     */
    void processMetadata(const std::string& errorName,
                         const std::vector<std::string>& additionalData,
                         AssociationList& objects) const;

    /** @brief Reads the BMC code level
     *
     *  @return std::string - the version string
     */
    static std::string readFWVersion();

    /** @brief Call any create() functions provided by any extensions.
     *  This is called right after an event log is created to allow
     *  extensions to create their own log based on this one.
     *
     *  @param[in] entry - the new event log entry
     *  @param[in] ffdc - A vector of FFDC file info
     */
    void doExtensionLogCreate(const Entry& entry, const FFDCEntries& ffdc);

    /** @brief Common wrapper for creating an Entry object
     *
     * @param[in] errMsg - The error exception message associated with the
     *                     error log to be committed.
     * @param[in] errLvl - level of the error
     * @param[in] additionalData - The AdditionalData property for the error
     * @param[in] ffdc - A vector of FFDC file info. Defaults to an empty
     * vector.
     */
    auto createEntry(std::string errMsg, Entry::Level errLvl,
                     std::map<std::string, std::string> additionalData,
                     const FFDCEntries& ffdc = FFDCEntries{})
        -> sdbusplus::message::object_path;

    /** @brief Notified on entry property changes
     *
     * If an entry is blocking, this callback will be registered to monitor for
     * the entry having it's Resolved field set to true. If it is then remove
     * the blocking object.
     *
     * @param[in] msg - sdbusplus dbusmessage
     */
    void onEntryResolve(sdbusplus::message_t& msg);

    /** @brief Remove block objects for any resolved entries  */
    void findAndRemoveResolvedBlocks();

    /** @brief Quiesce host if it is running
     *
     * This is called when the user has requested the system be quiesced
     * if a log with a callout is created
     */
    void checkAndQuiesceHost();

    /** @brief Persistent sdbusplus DBus bus connection. */
    sdbusplus::bus_t& busLog;

    /** @brief List of error ids for high severity errors */
    std::list<uint32_t> realErrors;

    /** @brief List of error ids for Info(and below) severity */
    std::list<uint32_t> infoErrors;

    /** @brief Id of last error log entry */
    uint32_t entryId;

    /** @brief The BMC firmware version */
    const std::string fwVersion;

    /** @brief Array of blocking errors */
    std::vector<std::unique_ptr<Block>> blockingErrors;

    /** @brief Map of entry id to call back object on properties changed */
    std::map<uint32_t, std::unique_ptr<sdbusplus::bus::match_t>>
        propChangedEntryCallback;

    /** @brief Encodes the BMC position in the entryId when enabled */
    std::unique_ptr<BMCPosMgr> bmcPosMgr;
};

} // namespace internal

/** @class Manager
 *  @brief Implementation for deleting all error log entries and
 *         creating new logs.
 *  @details A concrete implementation for the
 *           xyz.openbmc_project.Collection.DeleteAll and
 *           xyz.openbmc_project.Logging.Create interfaces.
 */
class Manager : public details::ServerObject<DeleteAllIface, CreateIface>
{
  public:
    Manager() = delete;
    Manager(const Manager&) = delete;
    Manager& operator=(const Manager&) = delete;
    Manager(Manager&&) = delete;
    Manager& operator=(Manager&&) = delete;
    virtual ~Manager() = default;

    /** @brief Constructor to put object onto bus at a dbus path.
     *         Defer signal registration (pass true for deferSignal to the
     *         base class) until after the properties are set.
     *  @param[in] bus - Bus to attach to.
     *  @param[in] path - Path to attach at.
     *  @param[in] manager - Reference to internal manager object.
     */
    Manager(sdbusplus::bus_t& bus, const std::string& path,
            internal::Manager& manager) :
        details::ServerObject<DeleteAllIface, CreateIface>(
            bus, path.c_str(),
            details::ServerObject<DeleteAllIface,
                                  CreateIface>::action::defer_emit),
        manager(manager) {};

    /** @brief Delete all d-bus objects.
     */
    void deleteAll() override
    {
        log<level::INFO>("Deleting all log entries");
        auto numbersOfLogs = manager.eraseAll();
        manager.createFromEvent(
            LoggingCleared("NUMBER_OF_LOGS", numbersOfLogs));
    }

    /** @brief D-Bus method call implementation to create an event log.
     *
     * @param[in] errMsg - The error exception message associated with the
     *                     error log to be committed.
     * @param[in] severity - Level of the error
     * @param[in] additionalData - The AdditionalData property for the error
     */
    auto create(std::string message, Severity severity,
                std::map<std::string, std::string> additionalData)
        -> sdbusplus::message::object_path override
    {
        return manager.create(message, severity, additionalData);
    }

    /** @brief D-Bus method call implementation to create an event log with FFDC
     *
     * The same as create(), but takes an extra FFDC argument.
     *
     * @param[in] errMsg - The error exception message associated with the
     *                     error log to be committed.
     * @param[in] severity - Level of the error
     * @param[in] additionalData - The AdditionalData property for the error
     * @param[in] ffdc - A vector of FFDC file info
     */
    void createWithFFDCFiles(
        std::string message, Severity severity,
        std::map<std::string, std::string> additionalData,
        std::vector<std::tuple<CreateIface::FFDCFormat, uint8_t, uint8_t,
                               sdbusplus::message::unix_fd>>
            ffdc) override
    {
        manager.create(message, severity, additionalData, ffdc);
    }

  private:
    /** @brief This is a reference to manager object */
    internal::Manager& manager;
};

} // namespace logging
} // namespace phosphor
