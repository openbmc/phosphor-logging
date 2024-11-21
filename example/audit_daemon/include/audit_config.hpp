/**
 * @brief Phosphor Audit configuration JSON file parser
 *
 * This file is part of the phosphor-logging audit project.
 *
 */

#pragma once

#include <string>
#include <vector>
#include <tuple>

namespace phosphor {
namespace logging {
namespace audit {

class Config {
  public:
    void ShowConfigs();

  private:
    Config(const std::string& path);
    void ReadConfigurationFile();
    std::string configuration_file_path;
    bool enabled;
};

} // namespace audit
} // namespace logging
} // namespace phosphor
