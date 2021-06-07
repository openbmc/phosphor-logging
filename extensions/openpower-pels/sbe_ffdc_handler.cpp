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

#include <ekb/hwpf/fapi2/include/return_code_defs.H>
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
            // Process SBE file.
            parse(file.fd);
        }
    }
}

void SbeFFDC::parse(int fd)
{
    log<level::INFO>(
        fmt::format("SBE FFDC file fd:({}), skipping", fd).c_str());

    uint32_t ffdcBufOffset = 0;
    uint32_t pktCount = 0;
    sbeFfdcPacketType ffdcPkt;

    // get SBE FFDC data.
    auto ffdcData = util::readFD(fd);
    if (ffdcData.empty())
    {
        log<level::ERR>(
            fmt::format("Empty SBE FFDC file fd:({}), skipping", fd).c_str());
        return;
    }

    while ((ffdcBufOffset < ffdcData.size()) && (sbeMaxFfdcPackets != pktCount))
    {
        // Next un-extracted FFDC Packet
        fapiFfdcBufType* ffdc =
            (fapiFfdcBufType*)(ffdcData.data() + ffdcBufOffset);
        auto magicBytes = ntohs(ffdc->magic_bytes);
        auto lenWords = ntohs(ffdc->lengthinWords);
        auto fapiRc = ntohl(ffdc->fapiRc);

        auto msg = fmt::format("FFDC magic: {} length in words:{} Fapirc:{}",
                               magicBytes, lenWords, fapiRc);
        log<level::INFO>(msg.c_str());

        if (magicBytes != ffdcMagicCode)
        {
            log<level::ERR>("Invalid FFDC magic code in Header: Skipping ");
            return;
        }
        ffdcPkt.fapiRc = fapiRc;
        // Not interested in the first 2 words (these are not ffdc)
        auto pktLenWords = lenWords - (2 * ffdcPkgOneWord);
        ffdcPkt.ffdcLengthInWords = pktLenWords;
        if (pktLenWords)
        {
            // Memory freeing will be taking care by ffdcPkt structure
            // destructor
            ffdcPkt.ffdcData =
                (uint32_t*)malloc((pktLenWords * sizeof(uint32_t)));
            memcpy(ffdcPkt.ffdcData,
                   (((uint32_t*)(ffdc)) +
                    (2 * ffdcPkgOneWord)), // skip first 2 words
                   (pktLenWords * sizeof(uint32_t)));
        }
        else
        {
            log<level::ERR>("FFDC packet size is zero skipping");
            return;
        }
        fprintf(stderr, "fapirc : %x const: %x \n", ffdcPkt.fapiRc,
                fapi2::FAPI2_RC_PLAT_ERR_SEE_DATA);

        if (ffdcPkt.fapiRc != fapi2::FAPI2_RC_PLAT_ERR_SEE_DATA)
        {
            process(ffdcPkt);
        }
        ffdcBufOffset += ffdc->lengthinWords;
        ++pktCount;
    }
    if (pktCount == sbeMaxFfdcPackets)
    {
        log<level::ERR>(fmt::format("Received more than the limit of ({})"
                                    " FFDC packets, processing only ({})",
                                    sbeMaxFfdcPackets, pktCount)
                            .c_str());
    }
}

void SbeFFDC::process(const sbeFfdcPacketType& ffdcPkt)
{
    using json = nlohmann::json;

    // formated FFDC data structure after FFDC packet processing
    FFDC ffdc;

    try
    {
        // libekb provided wrapper function to convert SBE FFDC
        // in to known ffdc structure.
        libekb_get_sbe_ffdc(ffdc, ffdcPkt, procPos);
    }
    catch (...)
    {
        log<level::ERR>("libekb_get_sbe_ffdc failed, skipping ffdc processing");
        return;
    }

    // To store callouts details in json format as per pel expectation.
    json pelJSONFmtCalloutDataList;
    pelJSONFmtCalloutDataList = json::array();

    // To store other user data from FFDC.
    openpower::pels::phal::FFDCData ffdcUserData;

    // Get FFDC and required info to include in PEL
    openpower::pels::phal::convertFAPItoPELformat(
        ffdc, pelJSONFmtCalloutDataList, ffdcUserData);

    // Get callout information and sore in to file.
    auto calloutData = pelJSONFmtCalloutDataList.dump();
    util::TemporaryFile ffdcFile(calloutData.c_str(), calloutData.size());

    // Create json callout type pel FFDC file structre.
    PelFFDCfile pf;
    pf.format = openpower::pels::UserDataFormat::json;
    pf.subType = openpower::pels::jsonCalloutSubtype;
    pf.version = 0x01;
    pf.fd = ffdcFile.getFd();
    ffdcFiles.push_back(pf);

    // save the file path to delete the file after usage.
    paths.push_back(ffdcFile.getPath());

    // Format ffdc user data and create new file.
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
