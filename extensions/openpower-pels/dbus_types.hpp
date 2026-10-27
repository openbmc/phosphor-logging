#pragma once

#include <org/freedesktop/DBus/ObjectManager/common.hpp>

#include <cstdint>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace openpower::pels
{

using BiosAttributes = std::map<
    std::string,
    std::tuple<
        std::string, bool, std::string, std::string, std::string,
        std::variant<int64_t, std::string>, std::variant<int64_t, std::string>,
        std::vector<std::tuple<std::string, std::variant<int64_t, std::string>,
                               std::string>>>>;

using DBusObjectMap = sdbusplus::common::org::freedesktop::d_bus::
    ObjectManager::get_managed_objects_t::return_type;
using DBusInterfaceMap = DBusObjectMap::mapped_type;
using DBusPropertyMap = DBusInterfaceMap::mapped_type;
using DBusValue = DBusPropertyMap::mapped_type;
using DBusProperty = std::string;
using DBusInterface = std::string;
using DBusService = std::string;
using DBusPath = std::string;
using DBusInterfaceList = std::vector<DBusInterface>;
using DBusPathList = std::vector<DBusPath>;
using DBusSubTree =
    std::map<DBusPath, std::map<DBusService, DBusInterfaceList>>;

} // namespace openpower::pels
