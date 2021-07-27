#pragma once

#include <syslog.h>

namespace lg2
{

enum class level
{
    emergency = LOG_EMERG,
    alert = LOG_ALERT,
    critical = LOG_CRIT,
    error = LOG_ERR,
    warning = LOG_WARNING,
    notice = LOG_NOTICE,
    info = LOG_INFO,
    debug = LOG_DEBUG,
};

}
