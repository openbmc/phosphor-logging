#pragma once

#include <map>
#include <sdbusplus/bus.hpp>
#include <string>
#include <variant>
#include <vector>

namespace openpower::pels
{

using DBusValue =
    sdbusplus::message::variant<std::string, bool, std::vector<uint8_t>>;
using DBusProperty = std::string;
using DBusInterface = std::string;
using DBusService = std::string;
using DBusPath = std::string;
using DBusInterfaceList = std::vector<DBusInterface>;
using DBusPathList = std::vector<DBusPath>;
using DBusPropertyMap = std::map<DBusProperty, DBusValue>;
using DBusInterfaceMap = std::map<DBusInterface, DBusPropertyMap>;

} // namespace openpower::pels
