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
    writeConfig(value, port(), RSYSLOG_SERVER_CONFIG_FILE);
    auto result = NetworkClient::address(value);
    return result;
}

uint16_t Server::port(uint16_t value)
{
    writeConfig(address(), value, RSYSLOG_SERVER_CONFIG_FILE);
    auto result = NetworkClient::port(value);
    return result;
}

void Server::writeConfig(
                 const std::string& serverAddress,
                 uint16_t serverPort,
                 const char* filePath)
{
    if (serverPort && !serverAddress.empty())
    {
        std::fstream stream(filePath, std::fstream::out);
        // write '*.* @@remote-host:port'
        stream << "*.* @@" << serverAddress << ":" << serverPort;

        utils::restart();
    }
}

} // namespace rsyslog_config
} // namespace phosphor
