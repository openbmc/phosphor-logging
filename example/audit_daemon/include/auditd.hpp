#pragma once

#include <libaudit.h>

#include <cstdint>
#include <string>

namespace phosphor
{
namespace logging
{
namespace audit
{

struct Auditd
{
  public:
    Auditd();
    ~Auditd();

    int LogEvent(const std::string& msg,
                 const std::string& addr = std::string(),
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
