#pragma once

#include <sdbusplus/bus.hpp>
#include "elog_entry.hpp"
#include "xyz/openbmc_project/Logging/Internal/Manager/server.hpp"

namespace phosphor
{
namespace logging
{
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
                entryId(0) {};

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


    private:
        /** @brief Persistent sdbusplus DBus bus connection. */
        sdbusplus::bus::bus& busLog;

        /** @brief Persistent map of Entry dbus objects and their ID */
        std::map<uint32_t, std::unique_ptr<Entry>> entries;

        /** @brief Id of last error log entry */
        uint32_t entryId;
};

} // namespace logging
} // namespace phosphor
