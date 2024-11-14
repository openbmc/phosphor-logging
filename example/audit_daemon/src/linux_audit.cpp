/**
 * @brief The Linux Audit Wrapper interface
 *
 * This file is part of the phosphor-audit project.
 *
 * Copyright (c) 2024 GAGAR>N
 *
 */
#include <cstdint>
#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <cerrno>

#include "linux_audit.hpp"

namespace phosphor {
namespace logging {
namespace audit {

LinuxAudit::LinuxAudit() : _fd(-1) { _isInitSuccess = Init(); };

LinuxAudit::~LinuxAudit(){};

int LinuxAudit::LogEvent(const std::string &msg, const std::string &hostname,
                         const std::string &addr,
                         const std::string &tty) const {
  int rc;

  if ((rc = audit_log_user_message(_fd, AUDIT_TEST, msg.c_str(),
                                   hostname.c_str(), addr.c_str(), tty.c_str(),
                                   0)) <= 0) {
    std::cout << "linux audit: error writing log: " << std::strerror(errno);
  }

  return rc;
}

bool LinuxAudit::Init() {
  _fd = audit_open();
  if (_fd < 0) {
    std::cout << "linux audit: failed to initialize: " << std::strerror(errno);
    return false;
  }

  return true;
}

bool LinuxAudit::IsInitSuccess() const { return _isInitSuccess; };

} // namespace audit
} // namespace logging
} // namespace phosphor
