#include "server-conf.hpp"

#include "utils.hpp"
#include "xyz/openbmc_project/Common/error.hpp"

#include <phosphor-logging/elog.hpp>

#include <fstream>
#if __has_include("../../usr/include/phosphor-logging/elog-errors.hpp")
#include "../../usr/include/phosphor-logging/elog-errors.hpp"
#else
#include <phosphor-logging/elog-errors.hpp>
#endif
#include <arpa/inet.h>
#include <netdb.h>

#include <optional>
#include <string>

namespace phosphor
{
namespace rsyslog_config
{

namespace utils = phosphor::rsyslog_utils;
using namespace phosphor::logging;
using namespace sdbusplus::error::xyz::openbmc_project::common;

namespace internal
{

bool isIPv6Address(const std::string& addr)
{
    struct in6_addr result;
    return inet_pton(AF_INET6, addr.c_str(), &result) == 1;
}

std::optional<
    std::tuple<std::string, uint32_t, NetworkClient::TransportProtocol>>
    parseConfig(std::istream& ss)
{
    std::string line;
    std::getline(ss, line);

    std::string serverAddress;
    std::string serverPort;
    NetworkClient::TransportProtocol serverTransportProtocol =
        NetworkClient::TransportProtocol::TCP;

    // Ignore if line is commented
    if (!line.empty() && '#' != line.at(0))
    {
        //"*.* @@<address>:<port>" or
        //"*.* @@[<ipv6-address>:<port>"
        auto start = line.find('@');
        if (start == std::string::npos)
            return {};

        // Skip "*.* @@" or "*.* @"
        if (line.at(start + 1) == '@')
        {
            serverTransportProtocol = NetworkClient::TransportProtocol::TCP;
            start += 2;
        }
        else
        {
            serverTransportProtocol = NetworkClient::TransportProtocol::UDP;
            start++;
        }

        // Check if there is "[]", and make IPv6 address from it
        auto posColonLeft = line.find('[');
        auto posColonRight = line.find(']');
        if (posColonLeft != std::string::npos ||
            posColonRight != std::string::npos)
        {
            // It contains [ or ], so it should be an IPv6 address
            if (posColonLeft == std::string::npos ||
                posColonRight == std::string::npos)
            {
                // There either '[' or ']', invalid config
                return {};
            }
            if (line.size() < posColonRight + 2 ||
                line.at(posColonRight + 1) != ':')
            {
                // There is no ':', or no more content after ':', invalid config
                return {};
            }
            serverAddress = line.substr(posColonLeft + 1,
                                        posColonRight - posColonLeft - 1);
            serverPort = line.substr(posColonRight + 2);
        }
        else
        {
            auto pos = line.find(':');
            if (pos == std::string::npos)
            {
                // There is no ':', invalid config
                return {};
            }
            serverAddress = line.substr(start, pos - start);
            serverPort = line.substr(pos + 1);
        }
    }
    if (serverAddress.empty() || serverPort.empty())
    {
        return {};
    }
    try
    {
        return std::make_tuple(std::move(serverAddress), std::stoul(serverPort),
                               serverTransportProtocol);
    }
    catch (const std::exception& ex)
    {
        log<level::ERR>("Invalid config", entry("ERR=%s", ex.what()));
        return {};
    }
}

} // namespace internal

std::string Server::address(std::string value)
{
    using Argument = xyz::openbmc_project::common::InvalidArgument;
    std::string result{};

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

        writeConfig(value, port(), transportProtocol(), configFilePath.c_str());
        result = NetworkClient::address(value);
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
    uint16_t result{};

    try
    {
        auto serverPort = port();
        if (serverPort == value)
        {
            return serverPort;
        }

        writeConfig(address(), value, transportProtocol(),
                    configFilePath.c_str());
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

NetworkClient::TransportProtocol
    Server::transportProtocol(NetworkClient::TransportProtocol value)
{
    TransportProtocol result{};

    try
    {
        auto serverTransportProtocol = transportProtocol();
        if (serverTransportProtocol == value)
        {
            return serverTransportProtocol;
        }

        writeConfig(address(), port(), value, configFilePath.c_str());
        result = NetworkClient::transportProtocol(value);
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
    const std::string& serverAddress, uint16_t serverPort,
    NetworkClient::TransportProtocol serverTransportProtocol,
    const char* filePath)
{
    std::fstream stream(filePath, std::fstream::out);

    if (serverPort && !serverAddress.empty())
    {
        std::string type =
            (serverTransportProtocol == NetworkClient::TransportProtocol::UDP)
                ? "@"
                : "@@";
        // write '*.* @@<remote-host>:<port>' or '*.* @<remote-host>:<port>'
        if (internal::isIPv6Address(serverAddress))
        {
            stream << "*.* " << type << "[" << serverAddress
                   << "]:" << serverPort;
        }
        else
        {
            stream << "*.* " << type << serverAddress << ":" << serverPort;
        }
    }
    else // this is a disable request
    {
        // dummy action to avoid error 2103 on startup
        stream << "*.* /dev/null";
    }

    stream << std::endl;

    restart();
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
        log<level::ERR>("bad address", entry("ADDRESS=%s", address.c_str()),
                        entry("ERRNO=%d", result));
        return false;
    }

    freeaddrinfo(res);
    return true;
}

void Server::restore(const char* filePath)
{
    std::fstream stream(filePath, std::fstream::in);

    auto ret = internal::parseConfig(stream);
    if (ret)
    {
        NetworkClient::address(std::get<0>(*ret));
        NetworkClient::port(std::get<1>(*ret));
        NetworkClient::transportProtocol(std::get<2>(*ret));
    }
}

void Server::restart()
{
    utils::restart();
}

} // namespace rsyslog_config
} // namespace phosphor
