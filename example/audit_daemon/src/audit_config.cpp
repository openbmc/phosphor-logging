#include "audit_config.hpp"

#include <fmt/format.h>

#include <boost/property_tree/exceptions.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <phosphor-logging/elog-errors.hpp>
#include <phosphor-logging/elog.hpp>
#include <phosphor-logging/log.hpp>

#include <sstream>
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

using namespace std;
using namespace phosphor;
using namespace logging;

namespace pt = boost::property_tree;

template <typename T>
std::vector<T> as_vector(const pt::ptree& pt, const pt::ptree::key_type& key)
{
    std::vector<T> r;
    for (auto& item : pt.get_child(key))
        r.push_back(item.second.get_value<T>());
    return r;
}

Config::Config(const std::string& path) : configuration_file_path(path) {}

void Config::ReadConfigurationFile()
{
    pt::ptree loadPtreeRoot;
    pt::ptree temp;

    try
    {
        pt::read_json(configuration_file_path, loadPtreeRoot);

        temp = loadPtreeRoot.get_child("IPMI");
        temp.get_child("Enabled").get_value<bool>();
    }
    catch (const boost::property_tree::ptree_error& e)
    {
        log<level::INFO>(fmt::format("{}", e.what()).c_str());
    }
}

void Config::ShowConfigs()
{
    std::stringstream ss;

    // as an example
    ss << "\r\nConfigs:\r\n";
    ss << "\t\tIPMI enabled : " << (enabled ? "yes" : "no") << "\r\n";
    log<level::INFO>(fmt::format("{}", ss.str()).c_str());
}

namespace details
{} // namespace details

} // namespace audit
} // namespace logging
} // namespace phosphor
