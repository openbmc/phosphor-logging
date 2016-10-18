// This file was autogenerated.  Do not edit!
// See elog-gen.py for more details
#pragma once

#include <tuple>
#include <type_traits>
#include "log.hpp"

namespace phosphor
{

namespace logging
{

namespace _GETSCOM
{
struct DEV_ADDR
{
    static constexpr auto str = "DEV_ADDR=0x%.8X";
    static constexpr auto str_short = "DEV_ADDR";
    using type = std::tuple<std::decay_t<decltype(str)>,int>;
    explicit constexpr DEV_ADDR(int a) : _entry(entry(str, a)) {};
    type _entry;
};
struct DEV_ID
{
    static constexpr auto str = "DEV_ID=%u";
    static constexpr auto str_short = "DEV_ID";
    using type = std::tuple<std::decay_t<decltype(str)>,int>;
    explicit constexpr DEV_ID(int a) : _entry(entry(str, a)) {};
    type _entry;
};
struct DEV_NAME
{
    static constexpr auto str = "DEV_NAME=%s";
    static constexpr auto str_short = "DEV_NAME";
    using type = std::tuple<std::decay_t<decltype(str)>,const char *>;
    explicit constexpr DEV_NAME(const char * a) : _entry(entry(str, a)) {};
    type _entry;
};

}  // namespace _GETSCOM

struct GETSCOM
{
    static constexpr auto err_code = "xyz.openbmc_project.logging.GETSCOM";
    static constexpr auto err_msg = "Getscom call failed";
    static constexpr level L = level::ERR;
    using DEV_ADDR = _GETSCOM::DEV_ADDR;
    using DEV_ID = _GETSCOM::DEV_ID;
    using DEV_NAME = _GETSCOM::DEV_NAME;
    
    using metadata_types = std::tuple<DEV_ADDR, DEV_ID, DEV_NAME>;
};

namespace _FILE_NOT_FOUND
{
struct ERRNUM
{
    static constexpr auto str = "ERRNUM=0x%.4X";
    static constexpr auto str_short = "ERRNUM";
    using type = std::tuple<std::decay_t<decltype(str)>,int>;
    explicit constexpr ERRNUM(int a) : _entry(entry(str, a)) {};
    type _entry;
};
struct FILE_PATH
{
    static constexpr auto str = "FILE_PATH=%s";
    static constexpr auto str_short = "FILE_PATH";
    using type = std::tuple<std::decay_t<decltype(str)>,const char *>;
    explicit constexpr FILE_PATH(const char * a) : _entry(entry(str, a)) {};
    type _entry;
};
struct FILE_NAME
{
    static constexpr auto str = "FILE_NAME=%s";
    static constexpr auto str_short = "FILE_NAME";
    using type = std::tuple<std::decay_t<decltype(str)>,const char *>;
    explicit constexpr FILE_NAME(const char * a) : _entry(entry(str, a)) {};
    type _entry;
};

}  // namespace _FILE_NOT_FOUND

struct FILE_NOT_FOUND
{
    static constexpr auto err_code = "xyz.openbmc_project.logging.FILE_NOT_FOUND";
    static constexpr auto err_msg = "A required file was not found";
    static constexpr level L = level::INFO;
    using ERRNUM = _FILE_NOT_FOUND::ERRNUM;
    using FILE_PATH = _FILE_NOT_FOUND::FILE_PATH;
    using FILE_NAME = _FILE_NOT_FOUND::FILE_NAME;
    
    using metadata_types = std::tuple<ERRNUM, FILE_PATH, FILE_NAME>;
};


} // namespace logging

} // namespace phosphor
