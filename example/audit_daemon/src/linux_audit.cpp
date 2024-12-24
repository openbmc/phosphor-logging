/**
 * @brief The Linux Audit Wrapper interface
 *
 * This file is part of the phosphor-audit project.
 *
 */
#include "linux_audit.hpp"

#include <cerrno>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

namespace phosphor
{
namespace logging
{
namespace audit
{

LinuxAudit::LinuxAudit() : _fd(-1)
{
    _isInitSuccess = Init();
};

LinuxAudit::~LinuxAudit(){};

int LinuxAudit::LogEvent(const std::string& msg, const std::string& addr,
                         const std::string& hostname,
                         const std::string& tty) const
{
    int rc;

    if ((rc = audit_log_user_message(_fd, AUDIT_TEST, msg.c_str(),
                                     hostname.c_str(), addr.c_str(),
                                     tty.c_str(), 0)) <= 0)
    {
        std::cout << "linux auditd: error writing to logs: "
                  << std::strerror(errno);
    }

    return rc;
}

bool LinuxAudit::Init()
{
    _fd = audit_open();
    if (_fd < 0)
    {
        std::cout << "linux auditd: failed to initialize: "
                  << std::strerror(errno);
        return false;
    }

    return true;
}

bool LinuxAudit::IsInitSuccess() const
{
    return _isInitSuccess;
};

} // namespace audit
} // namespace logging
} // namespace phosphor
