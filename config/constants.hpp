#pragma once
#include <cstddef>

// These values were previously configured via autoconf but are never modified
// by anyone, so they are kept as plain constants in this header.

#define PROCESS_META 1

inline constexpr auto BMC_VERSION_FILE = "/etc/os-release";
inline constexpr auto BUSNAME_LOGGING = "xyz.openbmc_project.Logging";
inline constexpr auto BUSNAME_SYSLOG_CONFIG =
    "xyz.openbmc_project.Syslog.Config";
inline constexpr auto BUSPATH_REMOTE_LOGGING_CONFIG =
    "/xyz/openbmc_project/logging/config/remote";
inline constexpr auto CALLOUT_FWD_ASSOCIATION = "callout";
inline constexpr auto CALLOUT_REV_ASSOCIATION = "fault";
inline constexpr auto INVENTORY_ROOT = "/xyz/openbmc_project/inventory";
inline constexpr auto OBJ_ENTRY = "/xyz/openbmc_project/logging/entry";
inline constexpr auto OBJ_INTERNAL =
    "/xyz/openbmc_project/logging/internal/manager";
inline constexpr auto OBJ_LOGGING = "/xyz/openbmc_project/logging";
inline constexpr auto SYSTEMD_BUSNAME = "org.freedesktop.systemd1";
inline constexpr auto SYSTEMD_INTERFACE = "org.freedesktop.systemd1.Manager";
inline constexpr auto SYSTEMD_PATH = "/org/freedesktop/systemd1";

inline constexpr auto FIRST_CEREAL_CLASS_VERSION_WITH_FWLEVEL = "2";
inline constexpr auto FIRST_CEREAL_CLASS_VERSION_WITH_UPDATE_TS = "3";
inline constexpr auto FIRST_CEREAL_CLASS_VERSION_WITH_EVENTID = "4";
inline constexpr auto FIRST_CEREAL_CLASS_VERSION_WITH_RESOLUTION = "5";
inline constexpr auto FIRST_CEREAL_CLASS_VERSION_WITH_METADATA_DICT = "6";
inline constexpr auto FIRST_CEREAL_CLASS_VERSION_WITH_METADATA_VECTOR = "7";
inline constexpr std::size_t CLASS_VERSION = 7;
inline constexpr std::size_t JSON_FORMAT_VERSION = 1;
