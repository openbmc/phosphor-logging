#include "config.h"
#include "server-conf.hpp"
#include "utils.hpp"
#include <fstream>

namespace phosphor
{
namespace rsyslog_config
{

namespace utils = phosphor::rsyslog_utils;

std::string Server::address(std::string value)
{
    auto result = NetworkClient::address(value);
    writeConfig();
    return result;
}

uint16_t Server::port(uint16_t value)
{
    auto result = NetworkClient::port(value);
    writeConfig();
    return result;
}

void Server::writeConfig()
{
    auto serverPort = port();
    auto serverAddress = address();

    if (serverPort && !serverAddress.empty())
    {
        std::fstream stream(RSYSLOG_SERVER_CONFIG_FILE, std::fstream::out);
        // write '*.* @@remote-host:port'
        stream << "*.* @@" << serverAddress << ":" << serverPort;

        utils::restart();
    }
}

} // namespace rsyslog_config
} // namespace phosphor
