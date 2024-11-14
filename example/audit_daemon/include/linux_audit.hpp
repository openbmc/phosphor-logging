/**
 * @brief The Linux Audit Wrapper interface
 *
 * This file is part of the phosphor-logging audit project.
 *
 */

#pragma once

#include <cstdint>
#include <string>

#include <libaudit.h>

namespace phosphor {
namespace logging {
namespace audit {

struct LinuxAudit {
public:
  LinuxAudit();
  ~LinuxAudit();

  int LogEvent(const std::string& msg, const std::string& addr = std::string(),
               const std::string& hostname = std::string(),
               const std::string& tty = std::string()) const;
  bool Init();
  bool IsInitSuccess() const;

private:
  int _fd;
  bool _isInitSuccess;
};

} // namespace audit
} // namespace logging
} // namespace phosphor
