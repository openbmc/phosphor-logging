#include "config.h"
#include "server-conf.hpp"
#include "utils.hpp"
#include "xyz/openbmc_project/Common/error.hpp"
#include <fstream>
#include <phosphor-logging/log.hpp>
#include <phosphor-logging/elog.hpp>
#if __has_include("../../usr/include/phosphor-logging/elog-errors.hpp")
#include "../../usr/include/phosphor-logging/elog-errors.hpp"
#else
#include <phosphor-logging/elog-errors.hpp>
#endif
#include <netdb.h>
#include <arpa/inet.h>

namespace phosphor
{
namespace rsyslog_config
{

namespace utils = phosphor::rsyslog_utils;
using namespace phosphor::logging;
using namespace sdbusplus::xyz::openbmc_project::Common::Error;

std::string Server::address(std::string value)
{
    using Argument = xyz::openbmc_project::Common::InvalidArgument;
    std::string result {};

    try
    {
        auto serverAddress = address();
        if (serverAddress == value)
        {
            return serverAddress;
        }

        if (!value.empty() && !addressValid(value))
        {
            elog<InvalidArgument>(Argument::ARGUMENT_NAME("Address"),
                                  Argument::ARGUMENT_VALUE(value.c_str()));
        }

        writeConfig(value, port(), RSYSLOG_SERVER_CONFIG_FILE);
        result = std::move(NetworkClient::address(value));
    }
    catch (const InvalidArgument& e)
    {
        throw;
    }
    catch (const InternalFailure& e)
    {
        throw;
    }
    catch (const std::exception& e)
    {
        log<level::ERR>(e.what());
        elog<InternalFailure>();
    }

    return result;
}

uint16_t Server::port(uint16_t value)
{
    uint16_t result {};

    try
    {
        auto serverPort = port();
        if (serverPort == value)
        {
            return serverPort;
        }

        writeConfig(address(), value, RSYSLOG_SERVER_CONFIG_FILE);
        result = NetworkClient::port(value);
    }
    catch (const InternalFailure& e)
    {
        throw;
    }
    catch (const std::exception& e)
    {
        log<level::ERR>(e.what());
        elog<InternalFailure>();
    }

    return result;
}

void Server::writeConfig(
                 const std::string& serverAddress,
                 uint16_t serverPort,
                 const char* filePath)
{
    std::fstream stream(filePath, std::fstream::out);

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

bool Server::addressValid(const std::string& address)
{
    addrinfo hints{};
    addrinfo* res = nullptr;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_CANONNAME;

    auto result = getaddrinfo(address.c_str(), nullptr, &hints, &res);
    if (result)
    {
        log<level::ERR>("bad address",
                        entry("ADDRESS=%s", address.c_str()),
                        entry("ERRNO=%d", result));
        return false;
    }
    return true;
}

} // namespace rsyslog_config
} // namespace phosphor
