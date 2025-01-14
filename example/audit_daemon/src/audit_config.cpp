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

    //std::tuple<variant(uint32_t/pair<uint8_t+uint8_t>), rid, msg>

    //parseBmcwebJson(&jsonCfg);
    if (!jsonCfg.contains("BMCWEB"))
        return;

    auto ballowListCfg = jsonCfg["BMCWEB"];
    if (!ballowListCfg.contains("Enabled"))
        throw std::invalid_argument("Enabled should be true or false");

    if (ballowListCfg.contains("Allowlist"))
    {
        for (auto& pathId : ballowListCfg["Allowlist"])
        {
            std::tuple<uint32_t, uint32_t> t(
                pathId["pid"], pathId["rid"]);
            bmcwebAllowList.push_back(t);
            if (!messages.contains(pathId["rid"]))
                 messages[pathId["rid"]] = std::move(pathId["msg"]);
        }
    }

    //parseIpmiJson(&jsonCfg);
    if (!jsonCfg.contains("IPMI"))
        return;

    auto iallowListCfg = jsonCfg["IPMI"];
    if (!iallowListCfg.contains("Enabled"))
        throw std::invalid_argument("Enabled should be true or false");

    if (iallowListCfg.contains("Allowlist"))
    {
        for (auto& cmdNetFn : iallowListCfg["Allowlist"])
        {
            std::tuple<uint8_t, uint8_t, uint32_t> t(
                cmdNetFn["cmd"], cmdNetFn["netfn"], cmdNetFn["rid"]);
            ipmiAllowList.push_back(t);
            if (!messages.contains(cmdNetFn["rid"]))
                 messages[cmdNetFn["rid"]] = std::move(cmdNetFn["msg"]);
        }
    }
};

ipmiList Config::getIPMIAllowList()
{
    return ipmiAllowList;
}

bmcwebList Config::getBMCWEBAllowList()
{
    return bmcwebAllowList;
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
