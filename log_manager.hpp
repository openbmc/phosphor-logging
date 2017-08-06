#pragma once

#include <sdbusplus/bus.hpp>
#include <phosphor-logging/log.hpp>
#include "elog_entry.hpp"
#include "xyz/openbmc_project/Logging/Internal/Manager/server.hpp"

namespace phosphor
{
namespace logging
{

extern const std::map<std::string,std::vector<std::string>> g_errMetaMap;
extern const std::map<std::string,level> g_errLevelMap;
namespace sdbusRule = sdbusplus::bus::match::rules;
namespace details
{

template <typename T>
using ServerObject = typename sdbusplus::server::object::object<T>;

using ManagerIface =
    sdbusplus::xyz::openbmc_project::Logging::Internal::server::Manager;

} // namespace details

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
        Manager(sdbusplus::bus::bus& bus, const char* objPath) :
                details::ServerObject<details::ManagerIface>(bus, objPath),
                busLog(bus),
                entryId(0),
                entryCapMatch(
                    bus,
                    sdbusRule::member("PropertiesChanged") +
                    sdbusRule::path(
                        "/xyz/openbmc_project/control/host0/entry_cap") +
                    sdbusRule::argN(0, "xyz.openbmc_project.Control.Entry.Cap") +
                    sdbusRule::interface("org.freedesktop.DBus.Properties"),
                    std::bind(std::mem_fn(&Manager::entryCapChanged),
                              this, std::placeholders::_1))
                {};

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
        void commit(uint64_t transactionId, std::string errMsg) override;


        /** @brief Erase specified entry d-bus object
         *
         * @param[in] entryId - unique identifier of the entry
         */
        void erase(uint32_t entryId);

        /** @brief Construct error d-bus objects from their persisted
         *         representations.
         */
        void restore();

    private:
        /** @brief Call metadata handler(s), if any. Handlers may create
         *         associations.
         *  @param[in] errorName - name of the error
         *  @param[in] additionalData - list of metadata (in key=value format)
         *  @param[out] objects - list of error's association objects
         */
        void processMetadata(const std::string& errorName,
                             const std::vector<std::string>& additionalData,
                             AssociationList& objects) const;

        /** @brief Persistent sdbusplus DBus bus connection. */
        sdbusplus::bus::bus& busLog;

        /** @brief Callback for EntryCap setting change
         *
         * @param[in] msg - Data associated with pcap change signal
         */
        void entryCapChanged(sdbusplus::message::message& msg);

        /**@brief Erase entries if entry cap is reached
         */
        void checkEntryCapAndEraseEntries();

        /** @brief Look up DBUS service for input path/interface
         *
         * @param[in] path - DBUS path
         * @param[in] intf - DBUS interface
         *
         * @return Distinct service name for input path/interface
         */
        std::string getService(std::string path, std::string intf);

        /** @brief Persistent map of Entry dbus objects and their ID */
        std::map<uint32_t, std::unique_ptr<Entry>> entries;

        /** @brief Id of last error log entry */
        uint32_t entryId;

        /** @brief Used to subscribe to dbus EntryCap propety changes **/
        sdbusplus::bus::match_t entryCapMatch;
};

} // namespace logging
} // namespace phosphor
