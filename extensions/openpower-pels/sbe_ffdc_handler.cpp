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

#include "fapi_data_process.hpp"
#include "pel.hpp"
#include "temporary_file.hpp"

#include <fmt/format.h>

#include <iostream>
#include <map>
#include <phosphor-logging/log.hpp>
#include <string>

namespace openpower
{
namespace pels
{
namespace sbe
{

using namespace phosphor::logging;

SbeFFDC::SbeFFDC(const AdditionalData& aData, const PelFFDC& files) :
    ffdcFiles(files)
{
    log<level::INFO>("SBE FFDC processing requested");

    // SRC6 field in the additional data contains Processor position
    // assocaited to the SBE FFDC
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
            (file.subType == sbeFFDCSubtype))
        {
            // TODO Process SBE file.
        }
    }
}

void SbeFFDC::process(const sbeFfdcPacketType& ffdcPkt)
{
    using json = nlohmann::json;
    FFDC ffdc;

    libekb_get_sbe_ffdc(ffdc, ffdcPkt, procPos);

    // To store callouts details in json format as per pel expectation.
    json pelJSONFmtCalloutDataList;
    pelJSONFmtCalloutDataList = json::array();

    // To store other user data from ffdc.
    openpower::pels::phal::FFDCData ffdcUserData;

    // Get FFDC and required info to include in PEL
    openpower::pels::phal::convertFAPItoPELformat(
        ffdc, pelJSONFmtCalloutDataList, ffdcUserData);

    auto calloutData = pelJSONFmtCalloutDataList.dump();
    util::TemporaryFile ffdcFile(calloutData.c_str(), calloutData.size() + 1);

    PelFFDCfile pf;
    pf.format = openpower::pels::UserDataFormat::json;
    pf.subType = openpower::pels::jsonCalloutSubtype;
    pf.version = 0x01;
    pf.fd = ffdcFile.getFd();
    ffdcFiles.push_back(pf);

    paths.push_back(ffdcFile.getPath());

    std::string data;
    for (auto& d : ffdcUserData)
    {
        data += d.first + " = " + d.second + "\n";
    }
    util::TemporaryFile pelDataFile(data.c_str(), data.size());
    PelFFDCfile pdf;
    pdf.format = openpower::pels::UserDataFormat::text;
    pdf.version = 0x01;
    pdf.fd = pelDataFile.getFd();
    ffdcFiles.push_back(pdf);

    paths.push_back(pelDataFile.getPath());
}

} // namespace sbe
} // namespace pels
} // namespace openpower
