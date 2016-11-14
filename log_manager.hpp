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
    sdbusplus::server::xyz::openbmc_project::Logging::Manager;

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
        Manager(sdbusplus::bus::bus &&, const char *, const char*, const char*);

        /** @brief Start processing DBus messages. */
        void run() noexcept;

        /*
         * @fn commit()
         * @brief sd_bus Commit method implementation callback.
         * @details Create an error/event log based on transaction id and
         *          error message.
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
