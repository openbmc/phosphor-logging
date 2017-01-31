// This file was autogenerated.  Do not edit!
// See elog-gen.py for more details
#pragma once

#include <string>
#include <tuple>
#include <type_traits>
#include "log.hpp"

namespace phosphor
{

namespace logging
{

namespace example
{
namespace xyz
{
namespace openbmc_project
{
namespace Example
{
namespace _TestErrorTwo
{
struct DEV_ADDR
{
    static constexpr auto str = "DEV_ADDR=0x%.8X";
    static constexpr auto str_short = "DEV_ADDR";
    using type = std::tuple<std::decay_t<decltype(str)>,uint32_t>;
    explicit constexpr DEV_ADDR(uint32_t a) : _entry(entry(str, a)) {};
    type _entry;
};
struct DEV_ID
{
    static constexpr auto str = "DEV_ID=%u";
    static constexpr auto str_short = "DEV_ID";
    using type = std::tuple<std::decay_t<decltype(str)>,uint32_t>;
    explicit constexpr DEV_ID(uint32_t a) : _entry(entry(str, a)) {};
    type _entry;
};
struct DEV_NAME
{
    static constexpr auto str = "DEV_NAME=%s";
    static constexpr auto str_short = "DEV_NAME";
    using type = std::tuple<std::decay_t<decltype(str)>,const char*>;
    explicit constexpr DEV_NAME(const char* a) : _entry(entry(str, a)) {};
    type _entry;
};

}  // namespace _TestErrorTwo

struct TestErrorTwo
{
    static constexpr auto err_code = "TestErrorTwo";
    static constexpr auto err_msg = "This is test error two";
    static constexpr auto L = level::ERR;
    using DEV_ADDR = _TestErrorTwo::DEV_ADDR;
    using DEV_ID = _TestErrorTwo::DEV_ID;
    using DEV_NAME = _TestErrorTwo::DEV_NAME;
    using metadata_types = std::tuple<DEV_ADDR, DEV_ID, DEV_NAME>;
};
} // namespace Example
} // namespace openbmc_project
} // namespace xyz
} // namespace example

namespace example
{
namespace xyz
{
namespace openbmc_project
{
namespace Example
{
namespace _TestErrorOne
{
struct ERRNUM
{
    static constexpr auto str = "ERRNUM=0x%.4X";
    static constexpr auto str_short = "ERRNUM";
    using type = std::tuple<std::decay_t<decltype(str)>,uint16_t>;
    explicit constexpr ERRNUM(uint16_t a) : _entry(entry(str, a)) {};
    type _entry;
};
struct FILE_PATH
{
    static constexpr auto str = "FILE_PATH=%s";
    static constexpr auto str_short = "FILE_PATH";
    using type = std::tuple<std::decay_t<decltype(str)>,const char*>;
    explicit constexpr FILE_PATH(const char* a) : _entry(entry(str, a)) {};
    type _entry;
};
struct FILE_NAME
{
    static constexpr auto str = "FILE_NAME=%s";
    static constexpr auto str_short = "FILE_NAME";
    using type = std::tuple<std::decay_t<decltype(str)>,const char*>;
    explicit constexpr FILE_NAME(const char* a) : _entry(entry(str, a)) {};
    type _entry;
};

}  // namespace _TestErrorOne

struct TestErrorOne
{
    static constexpr auto err_code = "TestErrorOne";
    static constexpr auto err_msg = "this is test error one";
    static constexpr auto L = level::INFO;
    using ERRNUM = _TestErrorOne::ERRNUM;
    using FILE_PATH = _TestErrorOne::FILE_PATH;
    using FILE_NAME = _TestErrorOne::FILE_NAME;
    using DEV_ADDR = TestErrorTwo::DEV_ADDR;
    using DEV_ID = TestErrorTwo::DEV_ID;
    using DEV_NAME = TestErrorTwo::DEV_NAME;
    using metadata_types = std::tuple<ERRNUM, FILE_PATH, FILE_NAME, DEV_ADDR, DEV_ID, DEV_NAME>;
};
} // namespace Example
} // namespace openbmc_project
} // namespace xyz
} // namespace example

namespace example
{
namespace xyz
{
namespace openbmc_project
{
namespace Example
{
namespace _Foo
{
struct FOO_DATA
{
    static constexpr auto str = "FOO_DATA=%s";
    static constexpr auto str_short = "FOO_DATA";
    using type = std::tuple<std::decay_t<decltype(str)>,const char*>;
    explicit constexpr FOO_DATA(const char* a) : _entry(entry(str, a)) {};
    type _entry;
};

}  // namespace _Foo

struct Foo
{
    static constexpr auto err_code = "Foo";
    static constexpr auto err_msg = "this is test error Foo";
    static constexpr auto L = level::INFO;
    using FOO_DATA = _Foo::FOO_DATA;
    using ERRNUM = TestErrorOne::ERRNUM;
    using FILE_PATH = TestErrorOne::FILE_PATH;
    using FILE_NAME = TestErrorOne::FILE_NAME;
    using DEV_ADDR = TestErrorTwo::DEV_ADDR;
    using DEV_ID = TestErrorTwo::DEV_ID;
    using DEV_NAME = TestErrorTwo::DEV_NAME;
    using metadata_types = std::tuple<FOO_DATA, ERRNUM, FILE_PATH, FILE_NAME, DEV_ADDR, DEV_ID, DEV_NAME>;
};
} // namespace Example
} // namespace openbmc_project
} // namespace xyz
} // namespace example

namespace example
{
namespace xyz
{
namespace openbmc_project
{
namespace Example
{
namespace _Bar
{
struct BAR_DATA
{
    static constexpr auto str = "BAR_DATA=%s";
    static constexpr auto str_short = "BAR_DATA";
    using type = std::tuple<std::decay_t<decltype(str)>,const char*>;
    explicit constexpr BAR_DATA(const char* a) : _entry(entry(str, a)) {};
    type _entry;
};

}  // namespace _Bar

struct Bar
{
    static constexpr auto err_code = "Bar";
    static constexpr auto err_msg = "this is test error Bar";
    static constexpr auto L = level::INFO;
    using BAR_DATA = _Bar::BAR_DATA;
    using FOO_DATA = Foo::FOO_DATA;
    using ERRNUM = TestErrorOne::ERRNUM;
    using FILE_PATH = TestErrorOne::FILE_PATH;
    using FILE_NAME = TestErrorOne::FILE_NAME;
    using DEV_ADDR = TestErrorTwo::DEV_ADDR;
    using DEV_ID = TestErrorTwo::DEV_ID;
    using DEV_NAME = TestErrorTwo::DEV_NAME;
    using metadata_types = std::tuple<BAR_DATA, FOO_DATA, ERRNUM, FILE_PATH, FILE_NAME, DEV_ADDR, DEV_ID, DEV_NAME>;
};
} // namespace Example
} // namespace openbmc_project
} // namespace xyz
} // namespace example


} // namespace logging

} // namespace phosphor
