#pragma once

#include <sdbusplus/bus.hpp>
#include <phosphor-logging/log.hpp>
#include "elog_entry.hpp"
#include "xyz/openbmc_project/Logging/Internal/Manager/server.hpp"
#include "xyz/openbmc_project/Collection/server.hpp"

namespace phosphor
{
namespace logging
{

extern const std::map<std::string,std::vector<std::string>> g_errMetaMap;
extern const std::map<std::string,level> g_errLevelMap;

using CollectionIfaces = sdbusplus::server::object::object <
                         sdbusplus::xyz::openbmc_project::server::Collection >;

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
                capped(false) {};

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

        /** @brief  Erase all error log entries
         *
         */
        void eraseAll()
        {
            for (const auto& entry : entries)
            {
                erase(entry.first);
            }
        }


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

        /** @brief Persistent map of Entry dbus objects and their ID */
        std::map<uint32_t, std::unique_ptr<Entry>> entries;

        /** @brief Id of last error log entry */
        uint32_t entryId;

        /**
         * @brief Flag to log error for the first time when error cap is
         *      reached.
         * @details Flag used to log error message for the first time when the
         *      error cap value is reached. It is reset when user delete's error
         *      entries and total entries existing is less than the error cap
         *      value.
         */
        bool capped;
};

/** @class Collection
 *  @brief OpenBMC collection implementation.
 *  @details A concrete implementation for the
 *  xyz.openbmc_project.Collection.
 */
class Collection : public CollectionIfaces
{
    public:
        Collection() = delete;
        Collection(const Collection&) = delete;
        Collection& operator=(const Collection&) = delete;
        Collection(Collection&&) = delete;
        Collection& operator=(Collection&&) = delete;
        virtual ~Collection() = default;

        /** @brief Constructor to put object onto bus at a dbus path.
         *         Defer signal registration (pass true for deferSignal to the
         *         base class) until after the properties are set.
         *  @param[in] bus - Bus to attach to.
         *  @param[in] path - Path to attach at.
         *  @param[in] manager - Reference to manager object.
         */
        Collection(sdbusplus::bus::bus& bus,
                   const std::string& path,
                   Manager& manager) :
            CollectionIfaces(bus, path.c_str(), true),
            manager(manager) {};

        /** @brief Delete all d-bus objects.
         */
        void delete_() override;

    private:
        /** @brief This is a reference to manager object */
        Manager& manager;
};

} // namespace logging
} // namespace phosphor
