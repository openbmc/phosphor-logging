## Note that this file is not auto generated, it is what generates the
## elog-metadata-postprocess.cpp file
// This file was autogenerated.  Do not edit!
// See elog-gen.py for more details

#include <string>
#include <vector>
#include <functional>
#include <phosphor-logging/elog-errors.hpp>
#include "elog_meta.hpp"

using namespace phosphor::logging;
using namespace example::xyz::openbmc_project::example2::elog;
using namespace std::placeholders;

extern const std::map<metadata::Metadata,
                      std::function<metadata::associations::Type>> meta = {
% for key, value in metadata_process.items():
<%
        import inflection
        def sdbusplus_name(name):
            names = name.split(".")
            return "::".join([inflection.underscore(x) for x in names[:-2]]) + "::" + "::".join([x for x in names[-2:]]) 
        type = sdbusplus_name(value)
%>\
    {"${key}", metadata::associations::build<${type}>},
    % endfor
};
