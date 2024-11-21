/**
 * @brief The phosphor audit configuration JSON file parser
 *
 * This file is part of the phosphor-logging audit project.
 *
 */
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include <tuple>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/exceptions.hpp>

#include "audit_config.hpp"

namespace phosphor {
namespace logging {
namespace audit {

using namespace std;

namespace pt = boost::property_tree;

static const std::string conf_file_path = "/etc/phosphor-audit/auditd.json";

template <typename T>
std::vector<T> as_vector(pt::ptree const &pt, pt::ptree::key_type const &key) {
  std::vector<T> r;
  for (auto &item : pt.get_child(key))
    r.push_back(item.second.get_value<T>());
  return r;
}

Config::Config(const std::string &path) : configuration_file_path(path) {}

void Config::ReadConfigurationFile() {
  pt::ptree loadPtreeRoot;
  pt::ptree temp;

  Config *config = Config::GetInstance();

  try {
    pt::read_json(configuration_file_path, loadPtreeRoot);

    temp = loadPtreeRoot.get_child("IPMI");
    config->enabled = temp.get_child("Enabled").get_value<bool>();
  } catch (const boost::property_tree::ptree_error &e) {
    std::cout << e.what() << std::endl;
  }
}

void Config::ShowConfigs() {
  std::stringstream ss;

  // as an example
  ss << "\r\nConfigs:\r\n";
  ss << "\t\tIPMI enabled : " << (enabled ? "yes" : "no") << "\r\n";
  std::cout << ss.str() << std::endl;
}

Config *Config::instance = nullptr;

Config *Config::GetInstance() {
  if (instance == nullptr) {
    instance = new Config(conf_file_path);
    instance->ReadConfigurationFile();
  }

  return instance;
}

namespace details {} // namespace details

} // namespace audit
} // namespace logging
} // namespace phosphor
