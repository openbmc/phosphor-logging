#pragma once

#include <phosphor-logging/elog-errors.hpp>
#include <phosphor-logging/lg2.hpp>

#include <cstdint>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace phosphor
{
namespace logging
{
namespace audit
{
namespace config
{

using ipmiList = std::vector<std::tuple<uint8_t, uint8_t, std::string>>;
static constexpr auto confFilePath = "/etc/phosphor-audit/audit.json";

static constexpr auto busName = "xyz.openbmc_project.Logging.Audit";
static constexpr auto objPath = "/xyz/openbmc_project/logging/audit";
static constexpr auto objMgr = "/xyz/openbmc_project/logging/audit/manager";

class Config
{
  public:
    Config();

    void parseConfFile();

    ipmiList getAllowList();

  private:
    ipmiList allowList;
};

} // namespace config
} // namespace audit
} // namespace logging
} // namespace phosphor
