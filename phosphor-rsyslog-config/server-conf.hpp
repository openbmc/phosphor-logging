#pragma once

#include <sdbusplus/bus.hpp>
#include <sdbusplus/server/object.hpp>
#include "xyz/openbmc_project/Network/Client/server.hpp"

namespace phosphor
{
namespace rsyslog_config
{

using NetworkClient = sdbusplus::xyz::openbmc_project::Network::server::Client;
using Iface = sdbusplus::server::object::object<NetworkClient>;

/** @class Server
 *  @brief Configuration for rsyslog server
 *  @details A concrete implementation of the
 *  xyz.openbmc_project.Network.Client API, in order to
 *  provide remote rsyslog server's address and port.
 */
class Server : public Iface
{
    public:
        Server() = delete;
        Server(const Server&) = delete;
        Server& operator=(const Server&) = delete;
        Server(Server&&) = delete;
        Server& operator=(Server&&) = delete;
        virtual ~Server() = default;

        /** @brief Constructor to put object onto bus at a dbus path.
         *  @param[in] bus - Bus to attach to.
         *  @param[in] path - Path to attach at.
         */
        Server(sdbusplus::bus::bus& bus,
                      const std::string& path) :
            Iface(bus, path.c_str())
        {
        }

        using NetworkClient::address;
        using NetworkClient::port;

        /** @brief Override that updates rsyslog config file as well
         *  @param[in] value - remote server address
         *  @returns value of changed address
         */
        virtual std::string address(std::string value) override;

        /** @brief Override that updates rsyslog config file as well
         *  @param[in] value - remote server port
         *  @returns value of changed port
         */
        virtual uint16_t port(uint16_t value) override;

    private:
        /** @brief Update remote server address and port in
         *         rsyslog config file.
         *  @param[in] serverAddress - remote server address
         *  @param[in] serverPort - remote server port
         *  @param[in] filePath - rsyslog config file path
         */
        void writeConfig(
                 const std::string& serverAddress,
                 uint16_t serverPort,
                 const char* filePath);
};

} // namespace rsyslog_config
} // namespace phosphor
