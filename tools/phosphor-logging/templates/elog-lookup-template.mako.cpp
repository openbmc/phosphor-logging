## Note that this file is not auto generated, it is what generates the
## elog-lookup.cpp file
// This file was autogenerated.  Do not edit!
// See elog-gen.py for more details
#include <map>
#include <vector>
#include "log.hpp"

namespace phosphor
{

namespace logging
{

std::map<std::string,std::vector<std::string>> g_errMetaMap = {
    % for a in errors:
    <% meta_string = '\",\"'.join(meta[a]) %> \
    {"${errors[a]}",{"${meta_string}"}},
    % endfor
};

std::map<std::string,level> g_errLevelMap = {
    % for a in errors:
    <% meta_string = '\",\"'.join(meta[a]) %> \
    {"${errors[a]}",level::${error_lvl[errors[a]]}},
    % endfor
};

} // namespace logging

} // namespace phosphor
