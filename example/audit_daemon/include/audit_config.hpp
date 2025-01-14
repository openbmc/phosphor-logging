#pragma once

#include <string>
#include <tuple>
#include <vector>

namespace phosphor
{
namespace logging
{
namespace audit
{

static const std::string conf_file_path = "/etc/phosphor-audit/auditd.json";

class Config
{
  public:
    void ShowConfigs();
    void ReadConfigurationFile();
    Config(const std::string& path);

  private:
    std::string configuration_file_path;
    bool enabled;
};

} // namespace audit
} // namespace logging
} // namespace phosphor
