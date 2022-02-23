#pragma once

#include <phosphor-logging/log.hpp>

using namespace phosphor::logging;

namespace openpower::pels::trace
{

inline void info(const std::string& message)
{
    log<level::INFO>(message.c_str());
}

inline void error(const std::string& message)
{
    log<level::ERR>(message.c_str());
}

inline void warning(const std::string& message)
{
    log<level::WARNING>(message.c_str());
}

inline void debug(const std::string& message)
{
    log<level::DEBUG>(message.c_str());
}

} // namespace openpower::pels::trace
