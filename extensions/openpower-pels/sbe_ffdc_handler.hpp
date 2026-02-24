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
 *
 *   -------------------P10 proc-----------------------
 *   |        |  Byte 0   |  Byte 1  |  Byte 2    |    Byte 3   |
 *   |----------------------------------------------------------|
 *   | Word 0 | Magic Bytes : 0xFFDC | Length in words (N+4)    |
 *   | Word 1 | [Sequence ID]        | Command-Class | Command  |
 *   | Word 2 |           Return Code 0..31                     |
 *   | Word 3 |           FFDC Data – Word 0                    |
 *   |  ...                                                     |
 *   | Word N+3 |  FFDC Data – Word N                           |
 *    -----------------------------------------------------------
 *
 *   --------------POZ - PowerOdysseyZEE-----------------------
 *   First FFDC packet structure
 *   --------------------------------------------------------------
 *   |          |  Byte 0   |  Byte 1  |  Byte 2    |    Byte 3   |
 *   |------------------------------------------------------------|
 *   | Word 0   | Magic Bytes : 0xFBAD | Length in words (N+4)    |
 *   | Word 1   | [Sequence ID]        | Command-Class | Command  |
 *   | Word 2   | SLID                 | Severity      | Chip ID  |
 *   | Word 3   |           FAPI RC (HWP)                         |
 *   | Word 4   | HWP FFDC Dump Fields (Local FFDC | HW Register) |
 *   | Word 6   | Field ID 0(Local FFDC) | Field ID 0 Length      |
 *   | Word 7   |          Filed Data 0(Size1, data2)             |
 *   |  ...                                                       |
 *   | Word N   |          Filed Data N(Size1, data2)             |
 *   | Word N+1 | Field ID 1(HW Register) | Field ID 1 Length     |
 *   | Word N+2 |              Filed Data 0)                      |
 *   |  ...                                                       |
 *   | Word Y   |              Filed Data M)                      |
 *    -------------------------------------------------------------
 *   Second FFDC packet structure
 *   --------------------------------------------------------------
 *   |          |  Byte 0   |  Byte 1  |  Byte 2    |    Byte 3   |
 *   |------------------------------------------------------------|
 *   | Word 0   | Magic Bytes : 0xFBAD | Length in words (N+4)    |
 *   | Word 1   | [Sequence ID]        | Command-Class | Command  |
 *   | Word 2   | SLID                 | Severity      | Chip ID  |
 *   | Word 3   |       FAPI RC (PLAT ERR SEE DATA)               |
 *   | Word 4   | Primary Status       | Secondary Status         |
 *   | Word 5   |             FW commit ID                        |
 *   | Word 5   | Reserve | DD Major   | DD Minor      | Thread ID|
 *   | Word 4   |         SBE FFDC Dump Fields (Bitmaped)         |
 *   | Word 6   | Field ID 0           | Field ID 0 Length        |
 *   | Word 7   |              Filed Data 0                       |
 *   |  ...                                                       |
 *   | Word N   |              Filed Data N                       |
 *   | Word N+1 | Field ID 1           | Field ID 1 Length        |
 *   | Word N+2 |       Filed Data 0)                             |
 *   |  ...                                                       |
 *   | Word Y   |       Filed Data M)                             |
 *   | Word Y+1 | 0xCODE               | 0xA8         | 0x1       |
 *   | Word Y+2 | Primary Status       | Secondary Status         |
 *   | Word Y+3 |          Distance to Header                     |
 *    -------------------------------------------------------------
 *
 **/

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
     * return PelFFDC - pel formatted FFDC files.
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
     * @brief Chip position associated to SBE FFDC
     */
    uint32_t chipPos;

    /**
     * @brief Used to get type of ffdc
     */
    FFDC_TYPE ffdcType;

    /**
     * @brief Chip type associated to SBE FFDC
     */
    uint32_t chipType;
};

} // namespace sbe
} // namespace pels
} // namespace openpower
