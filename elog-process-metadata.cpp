// This file was autogenerated.  Do not edit!
// See elog-gen.py for more details

#include <string>
#include <vector>
#include <functional>
#include <phosphor-logging/elog-errors.hpp>
#include "elog_meta.hpp"

using namespace phosphor::logging;
using namespace example::xyz::openbmc_project::Example::Elog;
using namespace std::placeholders;

extern const std::map<metadata::Metadata,
                      std::function<metadata::associations::Type>> meta = {
    {"DEV_ID", metadata::associations::build<example::xyz::openbmc_project::Example::Elog::TestErrorTwo::DEV_ID>},
    {"CALLOUT_DEVICE_PATH_TEST", metadata::associations::build<example::xyz::openbmc_project::Example::Device::Callout::CALLOUT_DEVICE_PATH_TEST>},
};
