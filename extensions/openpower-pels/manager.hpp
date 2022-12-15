#pragma once

#include "config.h"

#include "data_interface.hpp"
#include "event_logger.hpp"
#include "host_notifier.hpp"
#include "journal.hpp"
#include "log_manager.hpp"
#include "paths.hpp"
#include "pel.hpp"
#include "registry.hpp"
#include "repository.hpp"

#include <org/open_power/Logging/PEL/Entry/server.hpp>
#include <org/open_power/Logging/PEL/server.hpp>
#include <sdbusplus/server.hpp>
#include <sdeventplus/event.hpp>
#include <sdeventplus/source/event.hpp>
#include <xyz/openbmc_project/Logging/Create/server.hpp>

namespace openpower
{
namespace pels
{

using PELInterface = sdbusplus::server::object_t<
    sdbusplus::org::open_power::Logging::server::PEL>;

/**
 * @brief PEL manager object
 */
class Manager : public PELInterface
{
  public:
    Manager() = delete;
    Manager(const Manager&) = default;
    Manager& operator=(const Manager&) = default;
    Manager(Manager&&) = default;
    Manager& operator=(Manager&&) = default;

    /**
     * @brief constructor
     *
     * @param[in] logManager - internal::Manager object
     * @param[in] dataIface - The data interface object
     * @param[in] creatorFunc - The function that EventLogger will
     *                          use for creating event logs
     */
    Manager(phosphor::logging::internal::Manager& logManager,
            std::unique_ptr<DataInterfaceBase> dataIface,
            EventLogger::LogFunction creatorFunc,
            std::unique_ptr<JournalBase> journal) :
        PELInterface(logManager.getBus(), OBJ_LOGGING),
        _logManager(logManager), _eventLogger(std::move(creatorFunc)),
        _repo(getPELRepoPath()),
        _registry(getPELReadOnlyDataPath() / message::registryFileName),
        _event(sdeventplus::Event::get_default()),
        _dataIface(std::move(dataIface)), _journal(std::move(journal))
    {
        for (const auto& entry : _logManager.entries)
        {
            setEntryPath(entry.first);
            setServiceProviderNotifyFlag(entry.first);
            // Create PELEntry interface and setup properties with their values
            createPELEntry(entry.first, true);
        }

        _repo.for_each(
            std::bind(&Manager::updateResolution, this, std::placeholders::_1));

        setupPELDeleteWatch();
    }

    /**
     * @brief constructor that enables host notification
     *
     * @param[in] logManager - internal::Manager object
     * @param[in] dataIface - The data interface object
     * @param[in] creatorFunc - The function that EventLogger will
     *                          use for creating event logs
     * @param[in] hostIface - The hostInterface object
     */
    Manager(phosphor::logging::internal::Manager& logManager,
            std::unique_ptr<DataInterfaceBase> dataIface,
            EventLogger::LogFunction creatorFunc,
            std::unique_ptr<JournalBase> journal,
            std::unique_ptr<HostInterface> hostIface) :
        Manager(logManager, std::move(dataIface), std::move(creatorFunc),
                std::move(journal))
    {
        _hostNotifier = std::make_unique<HostNotifier>(
            _repo, *(_dataIface.get()), std::move(hostIface));
    }

    /**
     * @brief Destructor
     */
    ~Manager();

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
     * @param[in] ffdc - A vector of FFDC file information
     */
    void create(const std::string& message, uint32_t obmcLogID,
                uint64_t timestamp, phosphor::logging::Entry::Level severity,
                const std::vector<std::string>& additionalData,
                const std::vector<std::string>& associations,
                const phosphor::logging::FFDCEntries& ffdc =
                    phosphor::logging::FFDCEntries{});

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

    /**
     * @brief Return a file descriptor to the raw PEL data
     *
     * Throws InvalidArgument if the PEL ID isn't found,
     * and InternalFailure if anything else fails.
     *
     * @param[in] pelID - The PEL ID to get the data for
     *
     * @return unix_fd - File descriptor to the file that contains the PEL
     */
    sdbusplus::message::unix_fd getPEL(uint32_t pelID) override;

    /**
     * @brief Returns data for the PEL corresponding to an OpenBMC
     *        event log.
     *
     * @param[in] obmcLogID - The OpenBMC event log ID
     *
     * @return vector<uint8_t> - The raw PEL data
     */
    std::vector<uint8_t> getPELFromOBMCID(uint32_t obmcLogID) override;

    /**
     * @brief The D-Bus method called when a host successfully processes
     *        a PEL.
     *
     * This D-Bus method is called from the PLDM daemon when they get an
     * 'Ack PEL' PLDM message from the host, which indicates the host
     * firmware successfully sent it to the OS and this code doesn't need
     * to send it to the host again.
     *
     * @param[in] pelID - The PEL ID
     */
    void hostAck(uint32_t pelID) override;

    /**
     * @brief D-Bus method called when the host rejects a PEL.
     *
     * This D-Bus method is called from the PLDM daemon when they get an
     * 'Ack PEL' PLDM message from the host with a payload that says
     * something when wrong.
     *
     * The choices are either:
     *  * Host Full - The host's staging area is full - try again later
     *  * Malrformed PEL - The host received an invalid PEL
     *
     * @param[in] pelID - The PEL ID
     * @param[in] reason - One of the above two reasons
     */
    void hostReject(uint32_t pelID, RejectionReason reason) override;

    /**
     * @brief D-Bus method to create a PEL/OpenBMC event log and
     *        return the created OpenBMC and PEL log IDs.
     *
     * The same as the CreateWithFFDCFiles method on the
     * xyz.openbmc_project.Logging.Create interface, except for
     * the return values.
     *
     * @param[in] message - The event log message property
     * @param[in] severity - The event log severity
     * @param[in] additionalData - The AdditionalData property
     * @param[in] ffdc - A vector of FFDC file information
     */
    std::tuple<uint32_t, uint32_t> createPELWithFFDCFiles(
        std::string message, phosphor::logging::Entry::Level severity,
        std::map<std::string, std::string> additionalData,
        std::vector<std::tuple<sdbusplus::xyz::openbmc_project::Logging::
                                   server::Create::FFDCFormat,
                               uint8_t, uint8_t, sdbusplus::message::unix_fd>>
            fFDC) override;

    /**
     * @brief D-Bus method to return the PEL in JSON format
     *
     * @param[in] obmcLogID - The OpenBMC entry log ID
     *
     * @return std::string - The fully parsed PEL in JSON
     */
    std::string getPELJSON(uint32_t obmcLogID) override;

    /**
     * @brief Converts the ESEL field in an OpenBMC event log to a
     *        vector of uint8_ts that just contains the PEL data.
     *
     * That data string looks like: "50 48 00 ab ..."
     *
     * Throws an exception on any failures.
     *
     * @param[in] esel - The ESEL string
     *
     * @return std::vector<uint8_t> - The contained PEL data
     */
    static std::vector<uint8_t> eselToRawData(const std::string& esel);

    /**
     * @brief Generate resolution string from the PEL
     *
     * @param[in] pel - The PEL to use
     */
    std::string getResolution(const openpower::pels::PEL& pel) const;

    /**
     * @brief Generate event ID from the PEL
     *
     * @param[in] pel - The PEL to use
     */
    std::string getEventId(const openpower::pels::PEL& pel) const;

    /** @brief Implementation for GetPELIdFromBMCLogId
     *
     *  Returns the PEL Id (aka Entry ID (EID)) based on the given
     *  BMC event log id.
     *
     *  @param[in] bmcLogId - The BMC event log id of the PEL to retrieve
     *                        the PEL id.
     *
     *  @return uint32_t - The Id of the PEL.
     *                     Throw "InvalidArgument" if not found.
     */
    uint32_t getPELIdFromBMCLogId(uint32_t bmcLogId) override;

    /** @brief Implementation for GetBMCLogIdFromPELId
     *
     *  Returns the BMC event log id based on the given PEL id
     *  (aka Entry ID (EID)).
     *
     *  @param[in] pelId - The PEL id to retrieve the BMC event log id.
     *
     *  @return uint32_t - The BMC event log id of the PEL.
     *                     Throw "InvalidArgument" if not found.
     */
    uint32_t getBMCLogIdFromPELId(uint32_t pelId) override;

    /**
     * @brief Update boot progress SRC based on severity 0x51, critical error
     *
     * @param[in] pel - The PEL to use
     */
    void updateProgressSRC(std::unique_ptr<openpower::pels::PEL>& pel) const;

    /**
     * @brief Converts unprintable characters from the passed
     *        in string to spaces so they won't crash D-Bus when
     *        used as a property value.
     *
     * @param[in] field - The field to fix
     *
     * @return std::string - The string without non printable characters.
     */
    static std::string sanitizeFieldForDBus(std::string field);

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
     * @param[in] ffdc - A vector of FFDC file information
     */
    void createPEL(const std::string& message, uint32_t obmcLogID,
                   uint64_t timestamp, phosphor::logging::Entry::Level severity,
                   const std::vector<std::string>& additionalData,
                   const std::vector<std::string>& associations,
                   const phosphor::logging::FFDCEntries& ffdc);

    /**
     * @brief Schedules a close of the file descriptor to occur from
     *        the event loop.
     *
     * Uses sd_event_add_defer
     *
     * @param[in] fd - The file descriptor to close
     */
    void scheduleFDClose(int fd);

    /**
     * @brief Closes the file descriptor passed in.
     *
     * This is called from the event loop to close FDs returned
     * from getPEL().
     *
     * @param[in] fd - The file descriptor to close
     * @param[in] source - The event source object used
     */
    void closeFD(int fd, sdeventplus::source::EventBase& source);

    /**
     * @brief Adds a PEL to the repository given its data
     *
     * @param[in] pelData - The PEL to add as a vector of uint8_ts
     * @param[in] obmcLogID - the OpenBMC event log ID
     */
    void addPEL(std::vector<uint8_t>& pelData, uint32_t obmcLogID);

    /**
     * @brief Adds the PEL stored in the ESEL field of the AdditionalData
     *        property of an OpenBMC event log to the repository.
     *
     * @param[in] esel - The ESEL AdditionalData contents
     * @param[in] obmcLogID - The OpenBMC event log ID
     */
    void addESELPEL(const std::string& esel, uint32_t obmcLogID);

    /**
     * @brief Converts the D-Bus FFDC method argument into a data
     *        structure understood by the PEL code.
     *
     * @param[in] ffdc - A vector of FFDC file information
     *
     * @return PelFFDC - The PEL FFDC data structure
     */
    PelFFDC convertToPelFFDC(const phosphor::logging::FFDCEntries& ffdc);

    /**
     * @brief Schedules a PEL repository prune to occur from
     *        the event loop.
     *
     * Uses sd_event_add_defer
     */
    void scheduleRepoPrune();

    /**
     * @brief Prunes old PELs out of the repository to save space.
     *
     * This is called from the event loop.
     *
     * @param[in] source - The event source object used
     */
    void pruneRepo(sdeventplus::source::EventBase& source);

    /**
     * @brief Sets up an inotify watch to watch for deleted PEL
     *        files.  Calls pelFileDeleted() when that occurs.
     */
    void setupPELDeleteWatch();

    /**
     * @brief Called when the inotify watch put on the repository directory
     *        detects a PEL file was deleted.
     *
     * Will tell the Repository class about the deleted PEL, and then tell
     * the log manager class to delete the corresponding OpenBMC event log.
     */
    void pelFileDeleted(sdeventplus::source::IO& io, int fd, uint32_t revents);

    /**
     * @brief Check if the input PEL should cause a quiesce of the system
     *
     * If QuiesceOnHwError is enabled within phosphor-settings and the PEL
     * from the host has a severity which is not SeverityType::nonError or
     * recovered then execute the quiesce and boot block logic.
     *
     * @param[in] pel - The PEL to check
     */
    void checkPelAndQuiesce(std::unique_ptr<openpower::pels::PEL>& pel);

    /**
     * @brief Update eventId D-bus property for this error log
     *
     * Update the eventId property of D-bus with SRC and hexwords from the
     * PEL created
     *
     * @param[in] pel - The PEL to use
     */
    void updateEventId(std::unique_ptr<openpower::pels::PEL>& pel);

    /**
     * @brief Sets the FilePath of the specified error log entry to the PEL file
     *        path.
     *
     * @param[in] obmcLogID - The OpenBMC entry log ID
     */
    void setEntryPath(uint32_t obmcLogID);

    /**
     * @brief Sets the serviceProviderNotify D-bus property of PEL.
     *
     * @param[in] obmcLogID - The OpenBMC entry log ID
     */
    void setServiceProviderNotifyFlag(uint32_t obmcLogID);

    /**
     * @brief Update resolution D-bus property for this error log
     *
     * Update the resolution property of D-bus with callouts extracted from PEL
     *
     * @param[in] pel - The PEL to use
     *
     * @return bool - false for Repositor::for_each().
     */
    bool updateResolution(const openpower::pels::PEL& pel);

    /**
     * @brief Check if the D-Bus severity property for the event log
     *        needs to be updated based on the final PEL severity,
     *        and update the property accordingly.
     *
     * @param[in] pel - The PEL to operate on.
     */
    void updateDBusSeverity(const openpower::pels::PEL& pel);

    /**
     * @brief Create PELEntry Interface with supported properties
     *
     * Create PELEntry Interface and update all the properties which are
     * supported
     *
     * @param[in] obmcLogID - The OpenBMC entry log ID
     * @param[in] skipIaSignal - If The InterfacesAdded signal should be
     *                           skipped after creating the interfaces.
     */
    void createPELEntry(uint32_t obmcLogID, bool skipIaSignal = false);

    /**
     * @brief Schedules the delete of the OpenBMC event log for when
     *        execution gets back to the event loop (uses sd_event_add_defer).
     *
     * @param[in] obmcLogID - The OpenBMC entry log ID
     */
    void scheduleObmcLogDelete(uint32_t obmcLogID);

    /**
     * @brief SD event callback to delete an OpenBMC event log
     *
     * @param[in] obmcLogID - The OpenBMC entry log ID
     */
    void deleteObmcLog(sdeventplus::source::EventBase&, uint32_t obmcLogID);

    /**
     * @brief Reference to phosphor-logging's Manager class
     */
    phosphor::logging::internal::Manager& _logManager;

    /**
     * @brief Handles creating event logs/PELs from within
     *        the PEL extension code
     */
    EventLogger _eventLogger;

    /**
     * @brief The PEL repository object
     */
    Repository _repo;

    /**
     * @brief The PEL message registry object
     */
    message::Registry _registry;

    /**
     * @brief The Event object this class uses
     */
    sdeventplus::Event _event;

    /**
     * @brief The API the PEL sections use to gather data
     */
    std::unique_ptr<DataInterfaceBase> _dataIface;

    /**
     * @brief Object used to read from the journal
     */
    std::unique_ptr<JournalBase> _journal;

    /**
     * @brief The map used to keep track of PEL entry pointer associated with
     *        event log.
     */
    std::map<std::string,
             std::unique_ptr<
                 sdbusplus::org::open_power::Logging::PEL::server::Entry>>
        _pelEntries;

    /**
     * @brief The HostNotifier object used for telling the
     *        host about new PELs
     */
    std::unique_ptr<HostNotifier> _hostNotifier;

    /**
     * @brief The event source for closing a PEL file descriptor after
     *        it has been returned from the getPEL D-Bus method.
     */
    std::unique_ptr<sdeventplus::source::Defer> _fdCloserEventSource;

    /**
     * @brief The even source for removing old PELs when the repo is
     *        running out of space to make room for new ones.
     */
    std::unique_ptr<sdeventplus::source::Defer> _repoPrunerEventSource;

    /**
     * @brief The event source for deleting an OpenBMC event log.
     *        Used when its corresponding PEL is invalid.
     */
    std::unique_ptr<sdeventplus::source::Defer> _obmcLogDeleteEventSource;

    /**
     * @brief The even source for watching for deleted PEL files.
     */
    std::unique_ptr<sdeventplus::source::IO> _pelFileDeleteEventSource;

    /**
     * @brief The file descriptor returned by inotify_init1() used
     *        for watching for deleted PEL files.
     */
    int _pelFileDeleteFD = -1;

    /**
     * @brief The file descriptor returned by inotify_add_watch().
     */
    int _pelFileDeleteWatchFD = -1;
};

} // namespace pels
} // namespace openpower
