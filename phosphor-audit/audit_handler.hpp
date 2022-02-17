#pragma once

#include <string>

namespace phosphor
{
namespace audit
{

class AuditHandler
{
  public:
    virtual void responseAction(const std::string& buffer) = 0;

    virtual ~AuditHandler()
    {
    }
};

} // namespace audit
} // namespace phosphor