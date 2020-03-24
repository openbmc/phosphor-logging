#pragma once

#include "registry.hpp"
#include "section_header.hpp"

#include <optional>

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
    const SectionHeader& header() const
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
    virtual void flatten(Stream& stream) const = 0;

    /**
     * @brief Get section in JSON. Derived classes to override when required to.
     * @return std::optional<std::string> - If a section comes with a JSON
     * representation, this would return the string for it.
     */
    virtual std::optional<std::string> getJSON() const
    {
        return std::nullopt;
    }

    /**
     * @brief Get section in JSON. Derived classes to override when required to.
     * @param[in] registry - Registry object reference
     * @return std::optional<std::string> - If a section comes with a JSON
     * representation, this would return the string for it.
     */
    virtual std::optional<std::string>
        getJSON(message::Registry& registry) const
    {
        return std::nullopt;
    }

    /**
     * @brief Shrinks a PEL section
     *
     * If this is even possible for a section depends on which section
     * it is.  If a section cannot be shrunk, it doesn't need to implement
     * shrink so it will just return false, meaning no shrinking was done.
     *
     * If a section can be shrunk, this function will be overridden in that
     * class.
     *
     * @param[in] newSize - The new size, in bytes, to shrink to
     *
     * @return bool - true if successful, false else
     */
    virtual bool shrink(size_t newSize)
    {
        return false;
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
