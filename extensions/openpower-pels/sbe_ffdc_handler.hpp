#pragma once

#include "additional_data.hpp"
#include "pel.hpp"

#include <libekb.H>

#include <filesystem>

namespace openpower
{
namespace pels
{
namespace sbe
{

// SBE FFDC sub type.
constexpr uint8_t sbeFFDCSubType = 0xCB;

/**
 *   @brief FFDC Package structure and definitions based on SBE chip-op spec.
 *
 *   SBE FFDC Starts with a header word (Word 0) that has an unique magic
 *   identifier code of 0xFFDC followed by the length of the FFDC package
 *   including the header itself. Word 1 contains a sequence id ,
 *   command-class and command fields.
 *   The sequence id field is ignored on the BMC side.
 *   Word 2 contains a 32 bit Return Code which acts like the key to the
 *   contents of subsequent FFDC Data Words (0-N).
 *
 *   A FFDC package can typically contain debug data from either:
 *    1. A failed hardware procedure (e.g. local variable values
 *       at point of failure) or
 *    2. SBE firmware (e.g. traces, attributes and other information).
 *    ___________________________________________________________
 *   |        |  Byte 0   |  Byte 1  |  Byte 2    |    Byte 3   |
 *   |----------------------------------------------------------|
 *   | Word 0 | Magic Bytes : 0xFFDC | Length in words (N+4)    |
 *   | Word 1 | [Sequence ID]        | Command-Class | Command  |
 *   | Word 2 |           Return Code 0..31                     |
 *   | Word 3 |           FFDC Data – Word 0                    |
 *   |  ...                                                     |
 *   | Word N+3 |  FFDC Data – Word N                           |
 *    -----------------------------------------------------------
 **/

constexpr uint32_t sbeMaxFfdcPackets = 20;
constexpr uint32_t ffdcPkgOneWord = 1;
const uint16_t ffdcMagicCode = 0xFFDC;

typedef struct
{
    uint32_t magic_bytes:16;
    uint32_t lengthinWords:16;
    uint32_t seqId:16;
    uint32_t cmdClass:8;
    uint32_t cmd:8;
    uint32_t fapiRc;
} __attribute__((packed)) fapiFfdcBufType;

using LogSeverity = phosphor::logging::Entry::Level;

/** @class SbeFFDC
 *
 * @brief This class provides higher level interface to process SBE ffdc
 * for PEL based error logging infrastructure.
 * Key Functionalities included here
 *    - Process the SBE FFDC data with the help of FAPI infrastructure and
 *      and create PEL required format Callout and user data for hardware
 *      procedure failures specific reason code
 *    - Add the user data section with SBE FFDC data to support SBE provided
 *      parser tool usage.
 *    - Any SBE FFDC processing will result additional log message in journal
 *      and will continue to create Error log with available data. This is to
 *      help user to analyse the failure.
 */
class SbeFFDC
{
  public:
    SbeFFDC() = delete;
    SbeFFDC(const SbeFFDC&) = delete;
    SbeFFDC& operator=(const SbeFFDC&) = delete;
    SbeFFDC(SbeFFDC&&) = delete;
    SbeFFDC& operator=(SbeFFDC&&) = delete;

    /**
     * @brief Constructor
     *
     * Create PEL required format data from SBE provided FFDC data.
     *
     * @param[in] data - The AdditionalData properties in this PEL event
     * @param[in] files - FFDC files that go into UserData sections
     */
    SbeFFDC(const AdditionalData& data, const PelFFDC& files);

    /**
     * @brief Destructor
     *
     * Deletes the temporary files
     */
    ~SbeFFDC()
    {
        try
        {
            for (const auto& [path, fd] : paths)
            {
                if (!path.empty())
                {
                    // Delete temporary file from file system
                    std::error_code ec;
                    std::filesystem::remove(path, ec);
                }

                if (fd != -1)
                {
                    close(fd);
                }
            }
            paths.clear();
        }
        catch (...)
        {
            // Destructors should not throw exceptions
        }
    }

    /**
     * @brief Helper function to return FFDC files information, which
     *        includes SBE FFDC specific callout information.
     *
     * return PelFFDC - pel formated FFDC files.
     */
    const PelFFDC& getSbeFFDC()
    {
        return ffdcFiles;
    }

    /**
     * @brief Helper function to get severity type
     *
     * @return severity type as informational for spare clock
     *         failure type ffdc. Otherwise null string.
     */
    std::optional<LogSeverity> getSeverity();

  private:
    /**
     * @brief Helper function to parse SBE FFDC file.
     *        parsing is based on the FFDC structure definition
     *        define initially in this file.
     *
     * @param fd  SBE ffdc file descriptor
     *
     * Any failure during the process stops the function
     * execution to support the raw SBE FFDC data based
     * PEL creation.
     */
    void parse(int fd);

    /**
     * @brief Helper function to process SBE FFDC packet.
     * This function call libekb function to process the
     * FFDC packet and convert in to known format for PEL
     * specific file creation. This function also creates
     * json callout file and text type file, which includes
     * the addition debug data included in SBE FFDC packet.
     *
     * @param ffdcPkt  SBE FFDC packet
     *
     * Any failure during the process stops the function
     * execution to support the raw SBE FFDC data based
     * PEL creation.
     */
    void process(const sbeFfdcPacketType& ffdcPkt);

    /**
     * @brief  Temporary files path and FD information created as part of FFDC
     *         processing.
     */
    std::vector<std::pair<std::filesystem::path, int>> paths;

    /**
     * @brief PEL FFDC files, which includes the user data sections and the
     *        added callout details if any, found during SBE FFDC processing.
     */
    PelFFDC ffdcFiles;

    /**
     * @brief Processor position associated to SBE FFDC
     */
    uint32_t procPos;

    /**
     * @brief Used to get type of ffdc
     */
    FFDC_TYPE ffdcType;
};

} // namespace sbe
} // namespace pels
} // namespace openpower
