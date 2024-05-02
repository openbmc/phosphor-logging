## Note that this file is not auto generated, it is what generates the
## elog-lookup.cpp file
// This file was autogenerated.  Do not edit!
// See elog-gen.py for more details
#include <map>
#include <vector>
#include <log_manager.hpp>
#include <phosphor-logging/log.hpp>

namespace phosphor
{

namespace logging
{

const std::map<std::string,std::vector<std::string>> g_errMetaMap = {
    % for name in errors:
<%
    meta_string = ""
    meta_list = []
    if(name in meta and meta[name]):
        meta_list = meta[name]
        meta_string = '\",\"'.join(meta_list)

    parent = parents[name]
    while parent:
        if (parent in meta and meta[parent]):
            parent_meta_short = '\",\"'.join(meta[parent])
            if (meta_string):
                meta_string = meta_string + "\",\"" + parent_meta_short
            else:
                meta_string = parent_meta_short
        parent = parents[parent]
    if ("example.xyz.openbmc_project" not in name):
        index = name.rfind('.')
        name = name[:index] + ".Error" + name[index:]
%>\
    %if (meta_string):
    {"${name}",{"${meta_string}"}},
    %else:
    {"${name}",{}},
    %endif
    % endfor
};

const std::map<std::string,level> g_errLevelMap = {
    % for a in errors:
<%
    name = a
    if ("example.xyz.openbmc_project" not in name):
        index = name.rfind('.')
        name = name[:index] + ".Error" + name[index:]
%>\
    {"${name}",level::${error_lvl[a]}},
    % endfor
};

} // namespace logging

} // namespace phosphor