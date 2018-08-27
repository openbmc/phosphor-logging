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
    auto serverAddress = address();
    if (serverAddress == value)
    {
        return serverAddress;
    }

    auto result = NetworkClient::address(value);
    writeConfig();
    return result;
}

uint16_t Server::port(uint16_t value)
{
    auto serverPort = port();
    if (serverPort == value)
    {
        return serverPort;
    }

    auto result = NetworkClient::port(value);
    writeConfig();
    return result;
}

void Server::writeConfig()
{
    auto serverPort = port();
    auto serverAddress = address();
    std::fstream stream(RSYSLOG_SERVER_CONFIG_FILE, std::fstream::out);

    if (serverPort && !serverAddress.empty())
    {
        // write '*.* @@<remote-host>:<port>'
        stream << "*.* @@" << serverAddress << ":" << serverPort;
    }
    else // this is a disable request
    {
        // write '#*.* @@remote-host:port'
        stream << "#*.* @@remote-host:port";
    }
    utils::restart();
}

} // namespace rsyslog_config
} // namespace phosphor
