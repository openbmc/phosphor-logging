#pragma once

#include "pel_values.hpp"
#include "section_header.hpp"

#include <nlohmann/json.hpp>

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
     * @brief Used to convert a secontion to Json Format.
     * Values are kept in hex format for these json arrays.
     * PelParser converts them further using lookup tabled.
     * Implemented by derived classes.
     */
    virtual const char* toJson() = 0;

    /**
     * @brief Used to get the hex dump of a part of a section.
     * Implemented by derived classes.
     */
    template <typename byte_type = std::uint8_t>
    std::string hexdump(const byte_type* buffer, std::size_t size,
                        const std::string& title = "HEXDUMP");
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
