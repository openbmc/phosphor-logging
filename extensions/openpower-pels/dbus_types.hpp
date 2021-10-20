#pragma once

#include <map>
#include <sdbusplus/bus.hpp>
#include <string>
#include <variant>
#include <vector>

namespace openpower::pels
{

using DBusValue =
    std::variant<std::string, bool, std::vector<uint8_t>,
                 std::vector<std::string>,
                 std::vector<std::tuple<std::string, std::string, std::string>>,
                 std::tuple<uint64_t, std::vector<uint8_t>>>;
using DBusProperty = std::string;
using DBusInterface = std::string;
using DBusService = std::string;
using DBusPath = std::string;
using DBusInterfaceList = std::vector<DBusInterface>;
using DBusPathList = std::vector<DBusPath>;
using DBusPropertyMap = std::map<DBusProperty, DBusValue>;
using DBusInterfaceMap = std::map<DBusInterface, DBusPropertyMap>;
using DBusSubTree =
    std::map<DBusPath, std::map<DBusService, DBusInterfaceList>>;

} // namespace openpower::pels
