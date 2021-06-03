#pragma once

#include "additional_data.hpp"
#include "pel.hpp"

namespace openpower
{
namespace pels
{
namespace sbe
{

// SBE FFDC sub type.
constexpr uint8_t sbeFFDCSubType = 0xCB;

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
            for (auto path : paths)
            {
                if (!path.empty())
                {
                    // Delete temporary file from file system
                    std::filesystem::remove(path);
                    // Clear path to indicate file has been deleted
                    path.clear();
                }
            }
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

  private:
    /**
     * @brief  Temporary files path information created as part of FFDC
     *         processing.
     */
    std::vector<std::filesystem::path> paths;

    /**
     * @brief PEL FFDC files, which includes the user data sections and the
     *        added callout details if any, found during SBE FFDC processing.
     */
    PelFFDC ffdcFiles;

    /**
     * @brief Processor position associated to SBE FFDC
     */
    uint32_t procPos;
};

} // namespace sbe
} // namespace pels
} // namespace openpower
