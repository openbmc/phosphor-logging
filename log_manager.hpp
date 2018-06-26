#pragma once

#include <list>
#include <sdbusplus/bus.hpp>
#include <phosphor-logging/log.hpp>
#include "elog_entry.hpp"
#include "xyz/openbmc_project/Logging/Internal/Manager/server.hpp"
#include "xyz/openbmc_project/Collection/DeleteAll/server.hpp"

namespace phosphor
{
namespace logging
{

extern const std::map<std::string,std::vector<std::string>> g_errMetaMap;
extern const std::map<std::string,level> g_errLevelMap;

using DeleteAllIface = sdbusplus::server::object::object <
                       sdbusplus::xyz::openbmc_project::Collection::server::DeleteAll >;

namespace details
{

template <typename T>
using ServerObject = typename sdbusplus::server::object::object<T>;

using ManagerIface =
    sdbusplus::xyz::openbmc_project::Logging::Internal::server::Manager;

} // namespace details

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
        Manager(sdbusplus::bus::bus& bus, const char* objPath) :
                details::ServerObject<details::ManagerIface>(bus, objPath),
                busLog(bus),
                entryId(0),
                fwVersion(readFWVersion()) {};

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
        void commitWithLvl(uint64_t transactionId, std::string errMsg,
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
         */
        void eraseAll()
        {
            auto iter = entries.begin();
            while (iter != entries.end())
            {
                auto entry = iter->first;
                ++iter;
                erase(entry);
            }
        }
        std::list<uint32_t> realErrors;
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

        /** @brief Synchronize unwritten journal messages to disk.
         *  @details This is the same implementation as the systemd command
         *  "journalctl --sync".
         */
        void journalSync();

        /** @brief Reads the BMC code level
         *
         *  @return std::string - the version string
         */
        static std::string readFWVersion();

        /** @brief Persistent sdbusplus DBus bus connection. */
        sdbusplus::bus::bus& busLog;

        /** @brief Persistent map of Entry dbus objects and their ID */
        std::map<uint32_t, std::unique_ptr<Entry>> entries;

        /** @brief List of error ids for high severity errors */
        //std::list<uint32_t> realErrors;

        /** @brief List of error ids for Info(and below) severity */
        std::list<uint32_t> infoErrors;

        /** @brief Id of last error log entry */
        uint32_t entryId;

        /** @brief The BMC firmware version */
        const std::string fwVersion;
};

} //namespace internal

/** @class Manager
 *  @brief Implementation for delete all error log entries.
 *  @details A concrete implementation for the
 *  xyz.openbmc_project.Collection.DeleteAll
 */
class Manager : public DeleteAllIface
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
        Manager(sdbusplus::bus::bus& bus,
                   const std::string& path,
                   internal::Manager& manager) :
            DeleteAllIface(bus, path.c_str(), true),
            manager(manager) {};

        /** @brief Delete all d-bus objects.
         */
        void deleteAll()
        {
            manager.eraseAll();
        }
    private:
        /** @brief This is a reference to manager object */
        internal::Manager& manager;
};

} // namespace logging
} // namespace phosphor
