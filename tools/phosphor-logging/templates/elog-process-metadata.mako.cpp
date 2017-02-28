## Note that this file is not auto generated, it is what generates the
## elog-metadata-postprocess.cpp file
// This file was autogenerated.  Do not edit!
// See elog-gen.py for more details

#include <string>
#include <vector>
#include <functional>
#include "elog_meta.hpp"
#include "elog-gen.hpp"

using namespace phosphor::logging::metadata;
using namespace phosphor::logging::example::xyz::openbmc_project::Example;
using namespace std::placeholders;

extern const std::map<Metadata, std::function<HandlerType>> meta = {
    % for m in metadata_process:
<%
        type = m.replace(".", "::")
%>\
    {"${m}", std::bind(handler<${type}>, _1, _2)},
    % endfor
};
