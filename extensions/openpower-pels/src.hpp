#pragma once

#include "additional_data.hpp"
#include "ascii_string.hpp"
#include "callouts.hpp"
#include "pel_types.hpp"
#include "section.hpp"
#include "stream.hpp"

namespace openpower
{
namespace pels
{

constexpr size_t numSRCHexDataWords = 8;
constexpr uint8_t srcSectionVersion = 0x02;

/**
 * @class SRC
 *
 * SRC stands for System Reference Code.
 *
 * This class represents the SRC sections in the PEL, of which there are 2:
 * primary SRC and secondary SRC.  These are the same structurally, the
 * difference is that the primary SRC must be the 3rd section in the PEL if
 * present and there is only one of them, and the secondary SRC sections are
 * optional and there can be more than one (by definition, for there to be a
 * secondary SRC, a primary SRC must also exist).
 *
 * This section consists of:
 * - An 8B header (Has the version, flags, hexdata word count, and size fields)
 * - 8 4B words of hex data
 * - An ASCII character string
 * - An optional subsection for Callouts
 */
class SRC : public Section
{
  public:
    enum HeaderFlags
    {
        additionalSections = 0x01
    };

    SRC() = delete;
    ~SRC() = default;
    SRC(const SRC&) = delete;
    SRC& operator=(const SRC&) = delete;
    SRC(SRC&&) = delete;
    SRC& operator=(SRC&&) = delete;

    /**
     * @brief Constructor
     *
     * Fills in this class's data fields from the stream.
     *
     * @param[in] pel - the PEL data stream
     */
    explicit SRC(Stream& pel);

    /**
     * @brief Flatten the section into the stream
     *
     * @param[in] stream - The stream to write to
     */
    void flatten(Stream& stream) override;

    /**
     * @brief Returns the SRC version, which is a different field
     *        than the version byte in the section header.
     *
     * @return uint8_t
     */
    uint8_t version() const
    {
        return _version;
    }

    /**
     * @brief Returns the flags byte
     *
     * @return uint8_t
     */
    uint8_t flags() const
    {
        return _flags;
    }

    /**
     * @brief Returns the hex data word count.
     *
     * Even though there always 8 words, this returns 9 due to previous
     * SRC version formats.
     *
     * @return uint8_t
     */
    uint8_t hexWordCount() const
    {
        return _wordCount;
    }

    /**
     * @brief Returns the size of the SRC section, not including the header.
     *
     * @return uint16_t
     */
    uint16_t size() const
    {
        return _size;
    }

    /**
     * @brief Returns the 8 hex data words.
     *
     * @return const std::array<uint32_t, numSRCHexDataWords>&
     */
    const std::array<uint32_t, numSRCHexDataWords>& hexwordData() const
    {
        return _hexData;
    }

    /**
     * @brief Returns the ASCII string
     *
     * @return std::string
     */
    std::string asciiString() const
    {
        return _asciiString->get();
    }

    /**
     * @brief Returns the callouts subsection
     *
     * If no callouts, this unique_ptr will be empty
     *
     * @return  const std::unique_ptr<src::Callouts>&
     */
    const std::unique_ptr<src::Callouts>& callouts() const
    {
        return _callouts;
    }

  private:
    /**
     * @brief Fills in the object from the stream data
     *
     * @param[in] stream - The stream to read from
     */
    void unflatten(Stream& stream);

    /**
     * @brief Says if this SRC has additional subsections in it
     *
     * Note: The callouts section is the only possible subsection.
     *
     * @return bool
     */
    inline bool hasAdditionalSections() const
    {
        return _flags & static_cast<uint8_t>(HeaderFlags::additionalSections);
    }

    /**
     * @brief Validates the section contents
     *
     * Updates _valid (in Section) with the results.
     */
    void validate() override;

    /**
     * @brief The SRC version field
     */
    uint8_t _version;

    /**
     * @brief The SRC flags field
     */
    uint8_t _flags;

    /**
     * @brief A byte of reserved data after the flags field
     */
    uint8_t _reserved1B;

    /**
     * @brief The hex data word count.
     *
     * To be compatible with previous versions of SRCs, this is
     * number of hex words (8) + 1 = 9.
     */
    uint8_t _wordCount;

    /**
     * @brief Two bytes of reserved data after the hex word count
     */
    uint16_t _reserved2B;

    /**
     * @brief The total size of the SRC section, not including the section
     *        header.
     */
    uint16_t _size;

    /**
     * @brief The SRC 'hex words'.
     *
     * In the spec these are referred to as SRC words 2 - 9 as words 0 and 1
     * are filled by the 8 bytes of fields from above.
     */
    std::array<uint32_t, numSRCHexDataWords> _hexData;

    /**
     * @brief The 32 byte ASCII character string of the SRC
     *
     * It is padded with spaces to fill the 32 bytes.
     * An example is:
     * "BD8D1234                        "
     *
     * That first word is what is commonly referred to as the refcode, and
     * sometimes also called an SRC.
     */
    std::unique_ptr<src::AsciiString> _asciiString;

    /**
     * @brief The callouts subsection.
     *
     * Optional and only created if there are callouts.
     */
    std::unique_ptr<src::Callouts> _callouts;
};

} // namespace pels
} // namespace openpower
