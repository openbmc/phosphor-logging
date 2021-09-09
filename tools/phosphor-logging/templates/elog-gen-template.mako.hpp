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

<% exceptions = [] %>\
% for name in errors:
<%
    if("example.xyz.openbmc_project" not in name):
        exception = name.replace(".", "::")
        exception = "sdbusplus::" + exception
        index = exception.rfind("::")
        exception = exception[:index] + "::Error::" + exception[index+2:]
        exceptions.append(exception)
%>\
% endfor
% for exception in sorted(set(exceptions)):
<%
    ns = exception.split("::")
    exception_name = ns[-1]
    ns = ns[:-1]
%>\
    % for s in ns:
namespace ${s}
{
    % endfor
struct ${exception_name};
    % for s in reversed(ns):
} // namespace ${s}
    % endfor
% endfor

namespace phosphor
{

namespace logging
{

    % for index, name in enumerate(errors):
<%
    ## Ex: name: xyz.openbmc_project.Error.Callout.Device
    namespaces = name.split('.')
    ## classname is the last name item (Device)
    classname = namespaces[-1]
    ## namespaces are all the name items except the last one
    namespaces = namespaces[:-1]
%>\
    % for s in namespaces:
namespace ${s}
{
    % endfor
namespace _${classname}
{
<%
    meta_list = []
    if(name in meta):
        meta_list = meta[name]
%>\

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
    explicit constexpr ${b}(${meta_data[b]['type']} a) : _entry(entry("${meta_data[b]['str']}", a)) {};
    type _entry;
};
    % endfor

} // namespace _${classname}
<%
    example_yaml = False
    if("example.xyz.openbmc_project" in name):
        example_yaml = True
%>\
<%
    meta_string = ""
    if(meta_list):
        meta_string = ', '.join(meta_list)
    parent_meta = []

    parent = parents[name]
    while parent:
        tmpparent = parent.split('.')
        ## Name is the last item
        parent_name = tmpparent[-1]
        ## namespaces are all the name items except the last one
        parent_namespace = '::'.join(tmpparent[:-1])
        parent_meta += [parent_namespace + "::" + parent_name + "::" +
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
    using ${b.split("::").pop()} = ${b};
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

% for s in reversed(namespaces):
} // namespace ${s}
% endfor

<%
    sdbusplus_name = name
    if not example_yaml :
        sdbusplus_name = "sdbusplus." + sdbusplus_name
        pos = sdbusplus_name.rfind(".")
        sdbusplus_name = (sdbusplus_name[:pos] + ".Error." +
                          sdbusplus_name[pos+1:])
    sdbusplus_type = sdbusplus_name.replace(".", "::")
    phosphor_type = sdbusplus_type
    if not example_yaml :
        phosphor_type = sdbusplus_type.replace("sdbusplus::", "")
        phosphor_type = phosphor_type.replace("Error::", "")
%>\
\
% if sdbusplus_type != phosphor_type:
namespace details
{

template <>
struct map_exception_type<${sdbusplus_type}>
{
    using type = ${phosphor_type};
};

} // namespace details
%endif

    % endfor

} // namespace logging

} // namespace phosphor
