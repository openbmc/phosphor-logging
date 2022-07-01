#pragma once

#include "section.hpp"
#include "stream.hpp"

namespace openpower::pels
{

/**
 * @class ExtendedUserData
 *
 * This represents the Extended User Data section in a PEL.  It is free form
 * data that the creator knows the contents of.  The component ID, version, and
 * sub-type fields in the section header are used to identify the format.
 *
 *  This section is used for one subsystem to add FFDC data to a PEL created
 *  by another subsystem.  It is basically the same as a UserData section,
 *  except it has the creator ID of the section creator stored in the section.
 *
 * The Section base class handles the section header structure that every
 * PEL section has at offset zero.
 */
class ExtendedUserData : public Section
{
  public:
    ExtendedUserData() = delete;
    ~ExtendedUserData() = default;
    ExtendedUserData(const ExtendedUserData&) = default;
    ExtendedUserData& operator=(const ExtendedUserData&) = default;
    ExtendedUserData(ExtendedUserData&&) = default;
    ExtendedUserData& operator=(ExtendedUserData&&) = default;

    /**
     * @brief Constructor
     *
     * Fills in this class's data fields from the stream.
     *
     * @param[in] pel - the PEL data stream
     */
    explicit ExtendedUserData(Stream& pel);

    /**
     * @brief Constructor
     *
     * Create a valid ExtendedUserData object with the passed in data.
     *
     * The component ID, subtype, and version are used to identify
     * the data to know which parser to call.
     *
     * @param[in] componentID - Component ID of the creator
     * @param[in] subType - The type of user data
     * @param[in] version - The version of the data
     */
    ExtendedUserData(uint16_t componentID, uint8_t subType, uint8_t version,
                     uint8_t creatorID, const std::vector<uint8_t>& data);

    /**
     * @brief Flatten the section into the stream
     *
     * @param[in] stream - The stream to write to
     */
    void flatten(Stream& stream) const override;

    /**
     * @brief Returns the size of this section when flattened into a PEL
     *
     * @return size_t - the size of the section
     */
    size_t flattenedSize()
    {
        return Section::flattenedSize() + sizeof(_creatorID) +
               sizeof(_reserved1B) + sizeof(_reserved2B) + _data.size();
    }

    /**
     * @brief Returns the section creator ID field.
     *
     * @return uint8_t - The creator ID
     */
    uint8_t creatorID() const
    {
        return _creatorID;
    }

    /**
     * @brief Returns the raw section data
     *
     * This doesn't include the creator ID.
     *
     * @return std::vector<uint8_t>&
     */
    const std::vector<uint8_t>& data() const
    {
        return _data;
    }

    /**
     * @brief Returns the section data updated with new data
     *
     * @param[in] subType - The type of user data
     * @param[in] componentID - Component ID of the creator
     * @param[in] newData - The new data
     *
     */
    void updateDataSection(const uint8_t subType, const uint16_t componentId,
                           const std::vector<uint8_t>& newData)
    {
        if (newData.size() >= 4)
        {
            size_t origDataSize{};

            // Update component Id & subtype in section header of ED
            _header.componentID = static_cast<uint16_t>(componentId);
            _header.subType = static_cast<uint8_t>(subType);

            if (newData.size() > _data.size())
            {
                // Don't allow section to get bigger
                origDataSize = _data.size();
                _data = newData;
                _data.resize(origDataSize);
            }
            else
            {
                // Use shrink to handle 4B alignment and update the header size
                auto status =
                    shrink(Section::flattenedSize() + 4 + newData.size());
                if (status)
                {
                    origDataSize = _data.size();
                    _data = newData;
                    _data.resize(origDataSize);
                }
            }
        }
    }

    /**
     * @brief Get the section contents in JSON
     *
     * @param[in] creatorID - Creator Subsystem ID - unused (see the .cpp)
     * @param[in] plugins - Vector of strings of plugins found in filesystem
     *
     * @return The JSON as a string if a parser was found,
     *         otherwise std::nullopt.
     */
    std::optional<std::string>
        getJSON(uint8_t creatorID,
                const std::vector<std::string>& plugins) const override;

    /**
     * @brief Shrink the section
     *
     * The new size must be between the current size and the minimum
     * size, which is 12 bytes.  If it isn't a 4 byte aligned value
     * the code will do the aligning before the resize takes place.
     *
     * @param[in] newSize - The new size in bytes
     *
     * @return bool - true if successful, false else.
     */
    bool shrink(size_t newSize) override;

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
     * @brief The subsystem creator ID of the code that
     *        created this section.
     */
    uint8_t _creatorID;

    /**
     * @brief Reserved
     */
    uint8_t _reserved1B;

    /**
     * @brief Reserved
     */
    uint16_t _reserved2B;

    /**
     * @brief The section data
     */
    std::vector<uint8_t> _data;
};

} // namespace openpower::pels
