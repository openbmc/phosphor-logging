## Note that this file is not auto generated, it is what generates the
## elog-gen.hpp file
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

    % for a in errors:
namespace _${errors[a]}
{
    % for b in meta[a]:
struct ${b}
{
    static constexpr auto str = "${meta_data[b]['str']}";
    static constexpr auto str_short = "${meta_data[b]['str_short']}";
    using type = std::tuple<std::decay_t<decltype(str)>,${meta_data[b]['type']}>;
    explicit constexpr ${b}(${meta_data[b]['type']} a) : _entry(entry(str, a)) {};
    type _entry;
};
    % endfor

}  // namespace _${errors[a]}

struct ${errors[a]}
{
    static constexpr auto err_code = "xyz.openbmc_project.logging.${errors[a]}";
    static constexpr auto err_msg = "${error_msg[errors[a]]}";
    static constexpr level L = level::${error_lvl[errors[a]]};
    % for b in meta[a]:
    using ${b} = _${errors[a]}::${b};
    % endfor
    <%
        meta_string = ', '.join(meta[a])
    %>
    using metadata_types = std::tuple<${meta_string}>;
};

    % endfor

} // namespace logging

} // namespace phosphor
