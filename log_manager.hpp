#pragma once

#include <sdbusplus/server.hpp>
#include "xyz/openbmc_project/Logging/Manager/server.hpp"

namespace phosphor
{
namespace logging
{
namespace details
{

template <typename T>
using ServerObject = typename sdbusplus::server::object::object<T>;

using ManagerIface =
    sdbusplus::xyz::openbmc_project::Logging::server::Manager;

} // namespace details

/** @class Manager
 *  @brief OpenBMC logging manager implementation.
 *  @details A concrete implementation for xyz.openbmc_project.Logging.Manager
 *  DBus API.
 */
class Manager final :
    public details::ServerObject<details::ManagerIface>
{
    public:
        Manager() = delete;
        Manager(const Manager&) = delete;
        Manager& operator=(const Manager&) = delete;
        Manager(Manager&&) = default;
        Manager& operator=(Manager&&) = default;
        ~Manager() = default;

        /** @brief Constructor for the Log Manager object
         *  @param[in] bus - DBus bus to attach to.
         *  @param[in] busname - Name of DBus bus to own.
         *  @param[in] obj - Object path to attach to.
         */
        Manager(sdbusplus::bus::bus&& bus,
                    const char* busname,
                    const char* obj);

        /** @brief Start processing DBus messages. */
        void run() noexcept;

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
        sdbusplus::bus::bus _bus;

        /** @brief sdbusplus org.freedesktop.DBus.ObjectManager reference. */
        sdbusplus::server::manager::manager _manager;
};

} // namespace logging
} // namespace phosphor
