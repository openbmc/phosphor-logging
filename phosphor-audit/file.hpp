#pragma once

#include "audit_handler.hpp"

#include <string>

namespace phosphor
{
namespace audit
{

constexpr static auto filePath = "/var/lib/log/audit";
constexpr static auto fileHeader = "audit.log";
constexpr static auto maxSize = 1024 * 1024; // 1M
constexpr static auto auditPath = "/var/lib/log/audit/audit.log";

class FileHandler : public AuditHandler
{
  public:
    virtual void responseAction(const std::string& buffer) override;

  private:
    std::string getFilePath(size_t len);

    int writeFile(const std::string& buffer);
};

} // namespace audit
} // namespace phosphor