#include "audit_config.hpp"

#include <fmt/format.h>

#include <boost/property_tree/exceptions.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <nlohmann/json.hpp>
#include <phosphor-logging/elog-errors.hpp>
#include <phosphor-logging/elog.hpp>
#include <phosphor-logging/log.hpp>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

namespace phosphor
{
namespace logging
{
namespace audit
{
namespace config
{

using json = nlohmann::json;
namespace fs = std::filesystem;

Config::Config()
{
    parseConfFile();
};

void Config::parseConfFile()
{
    if (!fs::exists(confFilePath))
    {
        lg2::error("Configuration file doesn't exist");
        return;
    }

    std::ifstream ifs(confFilePath);

    auto jsonCfg = json::parse(ifs);
    if (!jsonCfg.contains("IPMI"))
        return;

    auto allowListCfg = jsonCfg["IPMI"];
    if (!allowListCfg.contains("Enabled"))
        throw std::invalid_argument("Enabled should be true or false");

    if (allowListCfg.contains("Allowlist"))
    {
        for (auto& cmdNetFn : allowListCfg["Allowlist"])
        {
            std::tuple<uint8_t, uint8_t, uint32_t> t(
                cmdNetFn["cmd"], cmdNetFn["netfn"], cmdNetFn["rid"]);
            allowList.push_back(t);
            messages[cmdNetFn["rid"]] = std::move(cmdNetFn["msg"]);
        }
    }
};

ipmiList Config::getAllowList()
{
    return allowList;
}

std::string Config::getMessage(uint32_t requestId)
{
    if (messages.contains(requestId))
        return messages[requestId];
    return std::string("(︶︹︶)");
}

} // namespace config
} // namespace audit
} // namespace logging
} // namespace phosphor
