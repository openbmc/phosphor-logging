// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2021 IBM Corporation

extern "C"
{
#include <libpdbg.h>
}

#include "fapi_data_process.hpp"
#include "pel.hpp"
#include "sbe_ffdc_handler.hpp"
#include "temporary_file.hpp"

#include <ekb/hwpf/fapi2/include/return_code_defs.H>
#include <ekb/hwpf/fapi2/include/target_types.H>
#include <libekb.H>

#include <phosphor-logging/lg2.hpp>

#include <new>

namespace openpower
{
namespace pels
{
namespace sbe
{

constexpr uint32_t sbeMaxFfdcPackets = 20;
constexpr uint16_t p10FfdcMagicCode = 0xFFDC;
constexpr uint16_t pozFfdcMagicCode = 0xFBAD;
constexpr uint16_t p10FfdcSkipWords = 2;
constexpr uint16_t pozFfdcSkipWords = 3;

struct p10FfdcHeader
{
    uint32_t magic_bytes:16;
    uint32_t lengthinWords:16;
    uint32_t seqId:16;
    uint32_t cmdClass:8;
    uint32_t cmd:8;
    uint32_t fapiRc;
} __attribute__((packed));

struct pozFfdcHeader
{
    uint32_t magicByte:16;
    uint32_t lengthinWords:16;
    uint32_t seqId:16;
    uint32_t cmdClass:8;
    uint32_t cmd:8;
    uint32_t slid:16;
    uint32_t severity:8;
    uint32_t chipId:8;
    uint32_t fapiRc;
} __attribute__((packed));

using namespace phosphor::logging;

SbeFFDC::SbeFFDC(const AdditionalData& aData, const PelFFDC& files) :
    ffdcType(FFDC_TYPE_NONE), chipType(fapi2::TARGET_TYPE_PROC_CHIP)
{
    lg2::info("SBE FFDC processing requested");

    // SRC6 field in the additional data contains Processor position
    // associated to the SBE FFDC
    //"[0:15] chip position"
    auto src6 = aData.getValue("SRC6");
    if (src6 == std::nullopt)
    {
        lg2::error("Fail to extract SRC6 data: failing to get proc index");
        return;
    }
    try
    {
        chipPos = (std::stoi(src6.value()) & 0xFFFF0000) >> 16;
    }
    catch (const std::exception& err)
    {
        lg2::error("Conversion failure errormsg({ERR})", "ERR", err);
        return;
    }
    auto type = aData.getValue("CHIP_TYPE");
    if (type != std::nullopt)
    {
        try
        {
            chipType = std::stoi(type.value());
        }
        catch (const std::exception& err)
        {
            lg2::error("Conversion failure errormsg({ERR})", "ERR", err);
            return;
        }
    }

    if (files.empty())
    {
        lg2::info("SbeFFDC : No files found, skipping ffdc processing");
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
    lg2::info("SBE FFDC file fd:({FD}), parsing started", "FD", fd);

    uint32_t ffdcBufOffset = 0;
    uint32_t pktCount = 0;

    // get SBE FFDC data.
    auto ffdcData = util::readFD(fd);
    if (ffdcData.empty())
    {
        lg2::error("Empty SBE FFDC file fd:({FD}), skipping", "FD", fd);
        return;
    }

    while ((ffdcBufOffset < ffdcData.size()) && (sbeMaxFfdcPackets != pktCount))
    {
        sbeFfdcPacketType ffdcPkt;
        // Next un-extracted FFDC Packet
        uint16_t magicBytes =
            *(reinterpret_cast<uint16_t*>(ffdcData.data() + ffdcBufOffset));
        magicBytes = ntohs(magicBytes);
        uint32_t pktLenWords = 0;
        uint16_t lenWords = 0;
        if (magicBytes == p10FfdcMagicCode)
        {
            p10FfdcHeader* ffdc = reinterpret_cast<p10FfdcHeader*>(
                ffdcData.data() + ffdcBufOffset);
            lenWords = ntohs(ffdc->lengthinWords);
            auto fapiRc = ntohl(ffdc->fapiRc);

            lg2::info(
                "P10 FFDC magic: {MAGIC_BYTES} length in words:{LEN_WORDS} "
                "Fapirc:{FAPI_RC}",
                "MAGIC_BYTES", magicBytes, "LEN_WORDS", lenWords, "FAPI_RC",
                fapiRc);

            ffdcPkt.fapiRc = fapiRc;
            // Not interested in the first 2 words (these are not ffdc)
            pktLenWords = lenWords - p10FfdcSkipWords;
            ffdcPkt.ffdcLengthInWords = pktLenWords;
            if (pktLenWords)
            {
                ffdcPkt.ffdcData = new uint32_t[pktLenWords];
                memcpy(ffdcPkt.ffdcData,
                       ((reinterpret_cast<uint32_t*>(ffdc)) + p10FfdcSkipWords),
                       (pktLenWords * sizeof(uint32_t)));
            }
            else
            {
                lg2::error("FFDC packet size is zero skipping");
                return;
            }
            pktCount++;
        }
        else if (magicBytes == pozFfdcMagicCode)
        {
            pozFfdcHeader* ffdc = reinterpret_cast<pozFfdcHeader*>(
                ffdcData.data() + ffdcBufOffset);
            lenWords = ntohs(ffdc->lengthinWords);
            auto fapiRc = ntohl(ffdc->fapiRc);

            lg2::info(
                "P0Z FFDC magic: {MAGIC_BYTES} length in words:{LEN_WORDS} "
                "Fapirc:{FAPI_RC}",
                "MAGIC_BYTES", magicBytes, "LEN_WORDS", lenWords, "FAPI_RC",
                fapiRc);

            ffdcPkt.fapiRc = fapiRc;
            // Not interested in the first 3 words (these are not ffdc)
            pktLenWords = lenWords - pozFfdcSkipWords;
            ffdcPkt.ffdcLengthInWords = pktLenWords;
            if (pktLenWords)
            {
                ffdcPkt.ffdcData = new uint32_t[pktLenWords];
                memcpy(ffdcPkt.ffdcData,
                       ((reinterpret_cast<uint32_t*>(ffdc)) + pozFfdcSkipWords),
                       (pktLenWords * sizeof(uint32_t)));
            }
            else
            {
                lg2::error("FFDC packet size is zero skipping");
                return;
            }
        }
        else
        {
            lg2::error("Invalid FFDC magic code in Header: Skipping ");
            return;
        }

        // SBE FFDC processing is not required for SBE Plat errors RCs.
        // Plat errors processing is driven by SBE provided user data
        // plugins, which need to link with PEL tool infrastructure.
        if (ffdcPkt.fapiRc != fapi2::FAPI2_RC_PLAT_ERR_SEE_DATA)
        {
            process(ffdcPkt);
        }
        else
        {
            lg2::info("SBE FFDC: Internal FFDC packet");
        }

        // Update Buffer offset in Bytes
        ffdcBufOffset += lenWords * sizeof(uint32_t);
    }
    if (pktCount == sbeMaxFfdcPackets)
    {
        lg2::error("Received more than the limit of ({SBEMAXFFDCPACKETS}) FFDC "
                   "packets, processing only ({PKTCOUNT})",
                   "SBEMAXFFDCPACKETS", sbeMaxFfdcPackets, "PKTCOUNT",
                   pktCount);
    }
}

void SbeFFDC::process(const sbeFfdcPacketType& ffdcPkt)
{
    using json = nlohmann::json;

    // formatted FFDC data structure after FFDC packet processing
    FFDC ffdc;

    try
    {
        // libekb provided wrapper function to convert SBE FFDC
        // in to known ffdc structure.
        libekb_get_sbe_ffdc(ffdc, ffdcPkt, chipPos, chipType);
    }
    catch (...)
    {
        lg2::error("libekb_get_sbe_ffdc failed, skipping ffdc processing");
        return;
    }

    // update FFDC type class membeir for hwp specific packet
    // Assumption SBE FFDC contains only one hwp FFDC packet.
    ffdcType = ffdc.ffdc_type;

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

    // Create json callout type pel FFDC file structure.
    PelFFDCfile pf;
    pf.format = openpower::pels::UserDataFormat::json;
    pf.subType = openpower::pels::jsonCalloutSubtype;
    pf.version = 0x01;
    pf.fd = ffdcFile.getFd();
    ffdcFiles.push_back(pf);

    // save the file path to delete the file after usage.
    paths.emplace_back(ffdcFile.getPath(), pf.fd);

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
    pdf.subType = 0;
    ffdcFiles.push_back(pdf);

    paths.emplace_back(pelDataFile.getPath(), pdf.fd);
}

std::optional<LogSeverity> SbeFFDC::getSeverity()
{
    if (ffdcType == FFDC_TYPE_SPARE_CLOCK_INFO)
    {
        lg2::info(
            "Found spare clock error, changing severity to informational");
        return LogSeverity::Informational;
    }
    return std::nullopt;
}

} // namespace sbe
} // namespace pels
} // namespace openpower
