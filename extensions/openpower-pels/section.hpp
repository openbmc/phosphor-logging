#pragma once
#include "section_header.hpp"

namespace openpower
{
namespace pels
{
/**
 * @class Section
 *
 * The base class for a PEL section.  It contains the SectionHeader
 * as all sections start with it.
 *
 */
class Section
{
  public:
    Section() = default;
    virtual ~Section() = default;
    Section(const Section&) = default;
    Section& operator=(const Section&) = default;
    Section(Section&&) = default;
    Section& operator=(Section&&) = default;

    /**
     * @brief Returns a reference to the SectionHeader
     */
    SectionHeader& header()
    {
        return _header;
    }

    /**
     * @brief Says if the section is valid.
     */
    bool valid() const
    {
        return _valid;
    }

    /**
     * @brief Flatten the section into the stream
     *
     * @param[in] stream - The stream to write to
     */
    virtual void flatten(Stream& stream) = 0;
    /**
     * @brief Used to convert a section to Json Format.
     * Implemented by derived classes.
     */
    virtual bool toJSONChk() const
    {
        printf("toJSONChk in Section Class.\n");
        return false;
    }
    /**
     * @brief Get section in JSON
     *
     */
    virtual std::string getJSON() const
    {
        return "";
    }

  protected:
    /**
     * @brief Returns the flattened size of the section header
     */
    static constexpr size_t flattenedSize()
    {
        return SectionHeader::flattenedSize();
    }

    /**
     * @brief Used to validate the section.
     *
     * Implemented by derived classes.
     */
    virtual void validate() = 0;

    /**
     * @brief The section header structure.
     *
     * Filled in by derived classes.
     */
    SectionHeader _header;

    /**
     * @brief The section valid flag.
     *
     * This is determined by the derived class.
     */
    bool _valid = false;
};
} // namespace pels
} // namespace openpower
