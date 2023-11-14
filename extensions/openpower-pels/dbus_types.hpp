#pragma once

#include <sdbusplus/message.hpp>

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

using DBusValue =
    std::variant<std::string, bool, std::vector<uint8_t>,
                 std::vector<std::string>,
                 std::vector<std::tuple<std::string, std::string, std::string>>,
                 std::tuple<uint64_t, std::vector<uint8_t>>, BiosAttributes>;
using DBusProperty = std::string;
using DBusInterface = std::string;
using DBusService = std::string;
using DBusPath = std::string;
using DBusInterfaceList = std::vector<DBusInterface>;
using DBusPathList = std::vector<DBusPath>;
using DBusPropertyMap = std::map<DBusProperty, DBusValue>;
using DBusInterfaceMap = std::map<DBusInterface, DBusPropertyMap>;
using DBusObjectMap =
    std::map<sdbusplus::message::object_path, DBusInterfaceMap>;
using DBusSubTree =
    std::map<DBusPath, std::map<DBusService, DBusInterfaceList>>;

} // namespace openpower::pels
