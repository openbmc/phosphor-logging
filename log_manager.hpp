#pragma once

#include <sdbusplus/server.hpp>
#include <xyz/openbmc_project/Logging/Manager/server.hpp>

namespace phosphor
{
namespace logging
{

/** @class Manager
 *  @brief OpenBMC logging manager implementation.
 *  @details A concrete implementation for xyz.openbmc_project.Logging.Manager
 *  DBus API.
 */
class Manager
{
    public:
        Manager() = delete;
        Manager(const Manager&) = delete;
        Manager& operator=(const Manager&) = delete;
        Manager(Manager&&) = default;
        Manager& operator=(Manager&&) = default;
        ~Manager() = default;
        Manager(const char *, const char*, const char*);

        /** @brief Start processing DBus messages. */
        void run() noexcept;

        /** @brief sd_bus Commit method implementation callback. */
        void commit(int msgid, std::string emsg) override;

    private:
        /** @brief Persistent sdbusplus DBus bus connection. */
        sdbusplus::bus::bus _bus;
};

} // namespace logging
} // namespace phosphor
