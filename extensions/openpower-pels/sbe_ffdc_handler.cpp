/**
 * Copyright © 2019 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "sbe_ffdc_handler.hpp"

#include <fmt/format.h>

#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace pels
{
namespace sbe
{

using namespace phosphor::logging;

SbeFFDC::SbeFFDC(const AdditionalData& aData, const PelFFDC& files)
{
    log<level::INFO>("SBE FFDC processing requested");

    // SRC6 field in the additional data contains Processor position
    // associated to the SBE FFDC
    //"[0:15] chip position"
    auto src6 = aData.getValue("SRC6");
    if (src6 == std::nullopt)
    {
        log<level::ERR>("Fail to extract SRC6 data: failing to get proc index");
        return;
    }
    try
    {
        procPos = std::stoi((src6.value()).substr(0, 4));
    }
    catch (std::exception& err)
    {
        log<level::ERR>(
            fmt::format("Conversion failure errormsg({})", err.what()).c_str());
        return;
    }

    if (files.empty())
    {
        log<level::INFO>("SbeFFDC : No files found, skipping ffdc processing");
        return;
    }

    for (const auto& file : files)
    {
        if ((file.format == UserDataFormat::custom) &&
            (file.subType == sbeFFDCSubType))
        {
            // TODO Process SBE file.
        }
    }
}

} // namespace sbe
} // namespace pels
} // namespace openpower
