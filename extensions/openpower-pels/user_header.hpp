#pragma once

#include "elog_entry.hpp"
#include "registry.hpp"
#include "section.hpp"
#include "stream.hpp"

namespace openpower
{
namespace pels
{

static constexpr uint8_t userHeaderVersion = 0x01;

/**
 * @class UserHeader
 *
 * This represents the User Header section in a PEL.  It is required,
 * and it is always the second section.
 *
 * The Section base class handles the section header structure that every
 * PEL section has at offset zero.
 *
 * The fields in this class directly correspond to the order and sizes of
 * the fields in the section.
 */
class UserHeader : public Section
{
  public:
    UserHeader() = delete;
    ~UserHeader() = default;
    UserHeader(const UserHeader&) = default;
    UserHeader& operator=(const UserHeader&) = default;
    UserHeader(UserHeader&&) = default;
    UserHeader& operator=(UserHeader&&) = default;

    /**
     * @brief Constructor
     *
     * Creates a valid UserHeader with the passed in data.
     *
     * @param[in] entry - The message registry entry for this error
     * @param[in] severity - The OpenBMC event log severity for this error
     */
    UserHeader(const message::Entry& entry,
               phosphor::logging::Entry::Level severity);

    /**
     * @brief Constructor
     *
     * Fills in this class's data fields from the stream.
     *
     * @param[in] pel - the PEL data stream
     */
    explicit UserHeader(Stream& pel);

    /**
     * @brief Flatten the section into the stream
     *
     * @param[in] stream - The stream to write to
     */
    void flatten(Stream& stream) override;

    /**
     * @brief Returns the subsystem field.
     *
     * @return uint8_t - the subsystem
     */
    uint8_t subsystem() const
    {
        return _eventSubsystem;
    }

    /**
     * @brief Returns the event scope field.
     *
     * @return uint8_t - the event scope
     */
    uint8_t scope() const
    {
        return _eventScope;
    }

    /**
     * @brief Returns the severity field.
     *
     * @return uint8_t - the severity
     */
    uint8_t severity() const
    {
        return _eventSeverity;
    }

    /**
     * @brief Returns the event type field.
     *
     * @return uint8_t - the event type
     */
    uint8_t eventType() const
    {
        return _eventType;
    }

    /**
     * @brief Returns the problem domain field.
     *
     * @return uint8_t - the problem domain
     */
    uint8_t problemDomain() const
    {
        return _problemDomain;
    }

    /**
     * @brief Returns the problem vector field.
     *
     * @return uint8_t - the problem vector
     */
    uint8_t problemVector() const
    {
        return _problemVector;
    }

    /**
     * @brief Returns the action flags field.
     *
     * @return uint16_t - the action flags
     */
    uint16_t actionFlags() const
    {
        return _actionFlags;
    }

    /**
     * @brief Returns the size of this section when flattened into a PEL
     *
     * @return size_t - the size of the section
     */
    static constexpr size_t flattenedSize()
    {
        return Section::flattenedSize() + sizeof(_eventSubsystem) +
               sizeof(_eventScope) + sizeof(_eventSeverity) +
               sizeof(_eventType) + sizeof(_reserved4Byte1) +
               sizeof(_problemDomain) + sizeof(_problemVector) +
               sizeof(_actionFlags) + sizeof(_reserved4Byte2);
    }

  private:
    /**
     * @brief Fills in the object from the stream data
     *
     * @param[in] stream - The stream to read from
     */
    void unflatten(Stream& stream);

    /**
     * @brief Validates the section contents
     *
     * Updates _valid (in Section) with the results.
     */
    void validate() override;

    /**
     * @brief The subsystem associated with the event.
     */
    uint8_t _eventSubsystem;

    /**
     * @brief The event scope field.
     */
    uint8_t _eventScope;

    /**
     * @brief The event severity.
     */
    uint8_t _eventSeverity;

    /**
     * @brief The event type.
     */
    uint8_t _eventType;

    /**
     * @brief A reserved word placeholder
     */
    uint32_t _reserved4Byte1;

    /**
     * @brief The problem domain field.
     */
    uint8_t _problemDomain;

    /**
     * @brief The problem vector field.
     */
    uint8_t _problemVector;

    /**
     * @brief The action flags field.
     */
    uint16_t _actionFlags;

    /**
     * @brief The second reserved word placeholder.
     */
    uint32_t _reserved4Byte2;
};

} // namespace pels
} // namespace openpower
