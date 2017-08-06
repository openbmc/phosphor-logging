#pragma once

#include <sdbusplus/bus.hpp>
#include <phosphor-logging/log.hpp>
#include "elog_entry.hpp"
#include "xyz/openbmc_project/Logging/Internal/Manager/server.hpp"
#include "xyz/openbmc_project/Logging/Internal/ErrorCap/server.hpp"

namespace phosphor
{
namespace logging
{
extern const std::map<std::string,std::vector<std::string>> g_errMetaMap;
extern const std::map<std::string,level> g_errLevelMap;

namespace sdbusRule = sdbusplus::bus::match::rules;
namespace details
{

template <typename T1, typename T2>
using ServerObject = typename sdbusplus::server::object::object<T1, T2>;

using ManagerIface =
    sdbusplus::xyz::openbmc_project::Logging::Internal::server::Manager;

using ErrorCapIface =
    sdbusplus::xyz::openbmc_project::Logging::Internal::server::ErrorCap;

} // namespace details

/** @class Manager
 *  @brief OpenBMC logging manager implementation.
 *  @details A concrete implementation for the
 *  xyz.openbmc_project.Logging.Internal.Manager DBus API.
 */
class Manager : public details::ServerObject<details::ManagerIface,
                                             details::ErrorCapIface>
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
                details::ServerObject<details::ManagerIface,
                    details::ErrorCapIface>(bus, objPath),
                busLog(bus),
                entryId(0),
                errorCapErrorLogged(false) {};

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

        /** @brief Persistent map of Entry dbus objects and their ID */
        std::map<uint32_t, std::unique_ptr<Entry>> entries;

        /** @brief Id of last error log entry */
        uint32_t entryId;

        /**Flag to log error when error cap is reached */
        bool errorCapErrorLogged;
};
} // namespace logging
} // namespace phosphor
