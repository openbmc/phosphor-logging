#include "auditd.hpp"

#include <cerrno>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>
#include <system_error>
#include <vector>

namespace phosphor
{
namespace logging
{
namespace audit
{

Auditd::Auditd()
{
    _fd = audit_open();
    if (_fd < 0)
    {
        throw std::runtime_error("linux auditd failed to initialize");
    }
};

Auditd::~Auditd() {};

void Auditd::LogEvent(const std::string& msg, const std::string& addr,
                      const std::string& hostname, const std::string& tty) const
{
    int rc;
    std::error_code ec;

    if ((rc = audit_log_user_message(_fd, AUDIT_TEST, msg.c_str(),
                                     hostname.c_str(), addr.c_str(),
                                     tty.c_str(), 0)) <= 0)
    {
        ec = std::make_error_code(static_cast<std::errc>(rc));
        std::cerr << std::quoted(ec.message()) << std::endl;
    }
}

} // namespace audit
} // namespace logging
} // namespace phosphor
