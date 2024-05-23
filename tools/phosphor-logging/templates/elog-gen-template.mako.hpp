## Note that this file is not auto generated, it is what generates the
## elog-gen.hpp file
// This file was autogenerated.  Do not edit!
// See elog-gen.py for more details
#pragma once

#include <phosphor-logging/elog.hpp>
#include <phosphor-logging/log.hpp>
#include <sdbusplus/exception.hpp>
#include <string>
#include <tuple>
#include <type_traits>

<%
import inflection

def sdbusplus_name(name):
    if "example.xyz.openbmc_project" in name:
        names = name.split(".")
    else:
        names = ["sdbusplus", "error" ] + name.split(".")

    classname = inflection.camelize(names[-1])
    namespace_name = "::".join([inflection.underscore(x) for x in names[:-1]])

    return (namespace_name, classname)

def phosphor_name(name):
    namespace_name, classname = sdbusplus_name(name)
    namespace_name = namespace_name.replace("sdbusplus::error::", "")

    return (namespace_name, classname)

def old_phosphor_name(name):
    names = name.split(".")
    return ("::".join(names[:-1]), names[-1])

exceptions = sorted(
    set([x for x in errors if "example.xyz.openbmc_project" not in x]))
%>\
% for error in exceptions:
<%
    ns, exception_name = sdbusplus_name(error)
%>\
namespace ${ns}
{
struct ${exception_name};
} // namespace ${ns}
% endfor

namespace phosphor::logging
{

% for name in errors:
<%
    namespaces, classname = phosphor_name(name)
    meta_list = meta.get(name, [])
%>\
namespace ${namespaces}
{
    % if len(meta_list) != 0:
namespace _${classname}
{
        % for b in meta_list:
struct ${b}
{
    /*
     * We can't use -fsanitize=undefined if we declare a
     * 'static constexpr auto str' member, so don't. Instead, open-code the
     * mako template lookups.
     */
    static constexpr auto str_short = "${meta_data[b]['str_short']}";
    using type = std::tuple<std::decay_t<decltype("${meta_data[b]['str']}")>,${meta_data[b]['type']}>;
    explicit constexpr ${b}(${meta_data[b]['type']} a) : _entry(entry("${meta_data[b]['str']}", a)){}
    std::string to_string() const
    {
        std::array<char, 512> buf;
        /* ignore the truncation */
        int n = std::snprintf(buf.data(), buf.size(), "${meta_data[b]['str']}", std::get<1>(_entry));
        return std::string(buf.data(), n);
    }
    type _entry;
};
        % endfor
} // namespace _${classname}
    % endif
<%
    example_yaml = "example.xyz.openbmc_project" in name

    meta_string = ""
    if(meta_list):
        meta_string = ', '.join(meta_list)
    parent_meta = []

    parent = parents[name]
    while parent:
        parent_ns, parent_class = phosphor_name(parent)

        parent_meta += [parent_ns + "::" + parent_class + "::" +
                        p for p in meta[parent]]
        parent_meta_short = ', '.join(meta[parent])

        # The parent may have empty meta,
        # so only add parent meta when it exists
        if (parent_meta_short):
            if(meta_string):
                meta_string = meta_string + ", " + parent_meta_short
            else:
                meta_string = parent_meta_short
        parent = parents[parent]

    if example_yaml:
        error_type = classname + " : public sdbusplus::exception_t"
    else:
        error_type = classname
%>
struct ${error_type}
{
    % if example_yaml:
    static constexpr auto errName = "${name}";
    static constexpr auto errDesc = "${error_msg[name]}";
    % endif
    static constexpr auto L = level::${error_lvl[name]};
    % for b in meta_list:
    using ${b} = _${classname}::${b};
    % endfor
    % for b in parent_meta:
    using ${b.split("::")[-1]} =
        phosphor::logging::${b};
    % endfor
    using metadata_types = std::tuple<${meta_string}>;
    % if example_yaml:

    const char* name() const noexcept override
    {
        return errName;
    }

    const char* description() const noexcept override
    {
        return errDesc;
    }

    const char* what() const noexcept override
    {
        return errName;
    }

    int get_errno() const noexcept override
    {
        return EIO;
    }
    % endif
};

} // namespace ${namespaces}
<%
    old_ns, old_class = old_phosphor_name(name)
%>
    % if old_ns != namespaces or old_class != classname:
#ifndef SDBUSPP_REMOVE_DEPRECATED_NAMESPACE
namespace ${old_ns}
{
using ${old_class} =
    phosphor::logging::${namespaces}::${classname};
}
#endif
    % endif

    % if not example_yaml:
<%
    sdbusplus_ns, sdbusplus_class = sdbusplus_name(name)
%>\
namespace details
{

template <>
struct map_exception_type<${sdbusplus_ns}::${sdbusplus_class}>
{
    using type =
        phosphor::logging::${namespaces}::${classname};
};

} // namespace details
    %endif
% endfor
} // namespace phosphor::logging
