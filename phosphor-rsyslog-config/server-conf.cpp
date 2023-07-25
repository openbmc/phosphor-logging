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

#define SERIALIZE_ENUM(ENUM_TYPE, ...)                                         \
    bool enumToStr(const ENUM_TYPE& e, std::string& s)                         \
    {                                                                          \
        static_assert(std::is_enum<ENUM_TYPE>::value,                          \
                      #ENUM_TYPE " must be an enum!");                         \
        static constexpr const std::pair<ENUM_TYPE, const char*> m[] =         \
            __VA_ARGS__;                                                       \
        auto it = std::find_if(std::begin(m), std::end(m),                     \
                               [&e](const auto& pair) -> bool                  \
                               {                                               \
                                   return pair.first == e;                     \
                               });                                             \
        if (it == std::end(m))                                                 \
        {                                                                      \
            return false;                                                      \
        }                                                                      \
        s = it->second;                                                        \
        return true;                                                           \
    }                                                                          \
                                                                               \
    bool enumFromStr(const std::string& s, ENUM_TYPE& e)                       \
    {                                                                          \
        static_assert(std::is_enum<ENUM_TYPE>::value,                          \
                      #ENUM_TYPE " must be an enum!");                         \
        static constexpr const std::pair<ENUM_TYPE, const char*> m[] =         \
            __VA_ARGS__;                                                       \
        auto it = std::find_if(std::begin(m), std::end(m),                     \
                               [&s](const auto& pair) -> bool                  \
                               {                                               \
                                   return pair.second == s;                    \
                               });                                             \
        if (it == std::end(m))                                                 \
        {                                                                      \
            return false;                                                      \
        }                                                                      \
        e = it->first;                                                         \
        return true;                                                           \
    }

namespace phosphor
{
namespace rsyslog_config
{

namespace utils = phosphor::rsyslog_utils;
using namespace phosphor::logging;
using namespace sdbusplus::xyz::openbmc_project::Common::Error;

namespace internal
{

/** PriorityModifier enum to syslog string mapping */
SERIALIZE_ENUM(PriorityModifier, {
    std::make_pair(PriorityModifier::NoModifier, ""),
    std::make_pair(PriorityModifier::Equal,      "="),
    std::make_pair(PriorityModifier::NotEqual,   "!="),
    std::make_pair(PriorityModifier::LowerThan,  "!"),
});

/** Facility enum to syslog string mapping */
SERIALIZE_ENUM(Facility, {
    std::make_pair(Facility::Auth,     "auth"),
    std::make_pair(Facility::Authpriv, "authpriv"),
    std::make_pair(Facility::Cron,     "cron"),
    std::make_pair(Facility::Daemon,   "daemon"),
    std::make_pair(Facility::Kern,     "kern"),
    std::make_pair(Facility::LPR,      "lpr"),
    std::make_pair(Facility::Mail,     "mail"),
    std::make_pair(Facility::News,     "news"),
    std::make_pair(Facility::NTP,      "ntp"),
    std::make_pair(Facility::Security, "security"),
    std::make_pair(Facility::Syslog,   "syslog"),
    std::make_pair(Facility::User,     "user"),
    std::make_pair(Facility::UUCP,     "uucp"),
    std::make_pair(Facility::FTP,      "ftp"),
    std::make_pair(Facility::Console,  "console"),
    std::make_pair(Facility::Local0,   "local0"),
    std::make_pair(Facility::Local1,   "local1"),
    std::make_pair(Facility::Local2,   "local2"),
    std::make_pair(Facility::Local3,   "local3"),
    std::make_pair(Facility::Local4,   "local4"),
    std::make_pair(Facility::Local5,   "local5"),
    std::make_pair(Facility::Local6,   "local6"),
    std::make_pair(Facility::Local7,   "local7"),
    std::make_pair(Facility::All,      "*"),
});

/** Priority enum to syslog string mapping */
SERIALIZE_ENUM(Priority, {
    std::make_pair(Priority::Emergency,     "emerg"),
    std::make_pair(Priority::Alert,         "alert"),
    std::make_pair(Priority::Critical,      "crit"),
    std::make_pair(Priority::Error,         "err"),
    std::make_pair(Priority::Warning,       "warning"),
    std::make_pair(Priority::Notice,        "notice"),
    std::make_pair(Priority::Informational, "info"),
    std::make_pair(Priority::Debug,         "debug"),
    std::make_pair(Priority::All,           "*"),
    std::make_pair(Priority::None,          "none"),
});

bool isIPv6Address(const std::string& addr)
{
    struct in6_addr result;
    return inet_pton(AF_INET6, addr.c_str(), &result) == 1;
}

inline std::string& rtrim(std::string& s, const char *ws = " \t")
{
    s.erase(s.find_last_not_of(ws) + 1);
    return s;
}

inline std::string& ltrim(std::string& s, const char *ws = " \t")
{
    s.erase(0, s.find_first_not_of(ws));
    return s;
}

std::vector<std::string> split(const std::string& s, const char delim)
{
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(s);

    while (getline(ss, token, delim)){
        tokens.push_back(token);
    }

    return tokens;
}

// The syntax of rsyslog.conf allows you to write multi-line statements using
// '\' at the end of the line.
// This method reads the "real" line from rsyslog.conf.
std::string readRealLine(std::istream& stream)
{

    std::string line;
    std::string realline{};
    bool lineCont = false;

    while (std::getline(stream, line))
    {
        line = ltrim(line);
        line = rtrim(line);

        if (line.empty())
        {
            break;
        }

        lineCont = (line.back() == '\\');

        realline.append(line, 0, (lineCont) ? line.length() - 1 : line.length() );

        // read only one line
        if (!lineCont) break;
    }

    return realline;
}

std::optional<std::pair<std::string, std::string>>
    parseRule(const std::string& s)
{
    std::string selectors;
    std::string action;

    auto pos = s.find_first_of(" \t");
    if (pos == std::string::npos) {
        return {};
    }

    selectors = s.substr(0, pos);
    pos = s.find_last_of(" \t");
    action = s.substr(pos + 1);
    return std::make_pair(std::move(selectors), std::move(action));

}

std::optional<std::tuple<std::string, std::string, std::string>>
    parseSelector(const std::string& s)
{
    // only one '.' is allowed in selector
    auto pair = split(s, '.');
    if (pair.size() != 2)
    {
        return {};
    }

    auto facilities = pair[0];
    auto priority = pair[1];
    std::string modifier = "";

    auto pos = priority.find_first_not_of("!=");
    if (pos != std::string::npos) {
        // smth like modifier exist
        priority = priority.substr(pos);
        modifier = priority.substr(0, pos);
    }

    return std::make_tuple(facilities, modifier, priority);
}

std::vector<selectorType> selectorsToDbusFmt(std::string s)
{
    std::vector<selectorType> ret;

    // rsyslog selectors are case-insensitive
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c){ return std::tolower(c); });

    auto selectors = internal::split(s, ';');
    for (const auto& s : selectors)
    {
        auto opt = parseSelector(s);
        if (!opt.has_value())
        {
            return {};
        }

        auto [facils, modifier, prio] = *opt;
        auto facilities = internal::split(facils, ',');

        std::vector<Facility> fv;
        PriorityModifier m;
        Priority p;

        for (const auto& facility : facilities)
        {
            Facility f;
            if (!enumFromStr(facility, f))
            {
                return {};
            }
            fv.push_back(f);
        }

        if (!enumFromStr(modifier, m) ||
            !enumFromStr(prio, p))
        {
            return {};
        }

        ret.emplace_back(fv, m, p);
    }

    return ret;
}

std::vector<std::string> selectorsFromDbusFmt(
    const std::vector<selectorType>& selectors)
{
    std::vector<std::string> ret;

    for (const auto& s : selectors)
    {
        auto& [facilities, modifier, prio] = s;
        std::string syslogFmt;
        std::string m;
        std::string p;

        for (const auto& facility : facilities)
        {
            std::string f;
            if (!enumToStr(facility, f))
            {
                return {};
            }
            syslogFmt.append(f);
            syslogFmt.append(",");
        }
        if (syslogFmt.back() == ',')
        {
            syslogFmt.pop_back();
        }

        if (!enumToStr(modifier, m) ||
            !enumToStr(prio, p))
        {
            return {};
        }

        syslogFmt.append(".");
        syslogFmt.append(m);
        syslogFmt.append(p);
        ret.push_back(syslogFmt);
    }

    return ret;
}

std::optional<
    std::tuple<std::string, uint32_t, NetworkClient::TransportProtocol>>
    parseRemoteAction(const std::string& line)
{
    std::string serverAddress;
    std::string serverPort;
    NetworkClient::TransportProtocol serverTransportProtocol =
        NetworkClient::TransportProtocol::TCP;

    // Ignore if line is commented
    if (!line.empty())
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
    using Argument = xyz::openbmc_project::Common::InvalidArgument;
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

        writeConfig(value, port(), transportProtocol(),
                    selectors(), configFilePath.c_str());
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
                    selectors(), configFilePath.c_str());
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

        writeConfig(address(), port(), value,
                    selectors(), configFilePath.c_str());
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

std::vector<selectorType> Server::selectors(std::vector<selectorType> value)
{
    std::vector<selectorType> result{};

    try
    {
        auto selectors = FilterIface::selectors();
        if (selectors == value)
        {
            return selectors;
        }

        writeConfig(address(), port(), transportProtocol(),
                    value, configFilePath.c_str());
        result = FilterIface::selectors(value);
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
    const std::vector<selectorType>& selectors,
    const char* filePath)
{
    std::fstream stream(filePath, std::fstream::out);

    if (!selectors.empty())
    {
        // keep rsyslog configuration file readable
        auto _selectors = internal::selectorsFromDbusFmt(selectors);
        auto it = _selectors.begin();
        for (; it != _selectors.end() - 1; it++) {
            stream << *it << ";\\\n  ";
        }
        stream << *it << "\t\t";
    }
    else
    {
        stream << "*.* ";
    }

    if (serverPort && !serverAddress.empty())
    {
        std::string type =
            (serverTransportProtocol == NetworkClient::TransportProtocol::UDP)
                ? "@"
                : "@@";
        // write '*.* @@<remote-host>:<port>' or '*.* @<remote-host>:<port>'
        if (internal::isIPv6Address(serverAddress))
        {
            stream << type << "[" << serverAddress << "]:" << serverPort;
        }
        else
        {
            stream << type << serverAddress << ":" << serverPort;
        }
    }
    else // this is a disable request
    {
        // dummy action to avoid error 2103 on startup
        stream << "/dev/null";
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
    std::string line = internal::readRealLine(stream);

    if (line.empty() || line.at(0) == '#')
    {
        // OpenBMC phosphor-rsyslog-conf:
        // The rsyslog configuration file was originally intended to contain
        // only one configuration line. We continue this tradition.
        return;
    }

    auto rule = internal::parseRule(line);
    if (!rule)
    {
        return;
    }

    // Selectors
    auto selectors = internal::selectorsToDbusFmt(rule->first);
    if (!selectors.empty())
    {
        FilterIface::selectors(selectors);
    }

    // Action (TCP/UDP addr and port)
    auto ret = internal::parseRemoteAction(rule->second);
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
