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

using ipmiList = std::vector<std::tuple<uint8_t, uint8_t, uint32_t>>;
static constexpr auto confFilePath = "/etc/phosphor-audit/audit.json";

class Config
{
  public:
    Config();

    void parseConfFile();

    ipmiList getAllowList();
    std::string getMessage(uint32_t requestId);

  private:
    ipmiList allowList;
    std::map<uint32_t, std::string> messages;
};

} // namespace config
} // namespace audit
} // namespace logging
} // namespace phosphor
