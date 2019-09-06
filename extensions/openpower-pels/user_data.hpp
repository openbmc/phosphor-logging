#pragma once

#include "section.hpp"
#include "stream.hpp"

namespace openpower
{
namespace pels
{

static constexpr uint16_t userDataSectionID = 0x5544; // 'UD'
static constexpr uint16_t userDataVersion = 0x01;

/**
 * @class UserData
 *
 * This represents the User Data section in a PEL.
 *
 * The Section base class handles the section header structure that every
 * PEL section has at offset zero.
 *
 * The fields in this class directly correspond to the order and sizes of
 * the fields in the section.
 */
class UserData : public Section
{
  public:
    UserData() = delete;
    ~UserData() = default;
    UserData(const UserData&) = default;
    UserData& operator=(const UserData&) = default;
    UserData(UserData&&) = default;
    UserData& operator=(UserData&&) = default;

    /**
     * @brief Constructor
     *
     * Fills in this class's data fields from the stream.
     *
     * @param[in] pel - the PEL data stream
     */
    explicit UserData(Stream& pel);

    /**
     * @brief Flatten the section into the stream
     *
     * @param[in] stream - The stream to write to
     */
    void flatten(Stream& stream) override;

    /**
     * @brief Returns the size of this section when flattened into a PEL
     *
     * @return size_t - the size of the section
     */
    size_t flattenedSize()
    {
        return Section::flattenedSize() + _data.size();
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
     * @brief user data
     */
    std::vector<uint8_t> _data;
};
/**
 * @brief Stream extraction operator for vector<uint8_t>
 *
 * @param[in] s - the stream
 * @param[out] v - the vector of user data
 */
Stream& operator>>(Stream& s, vector<uint8_t>& v);

/**
 * @brief Stream insertion operator for vector<uint8_t>
 *
 * @param[in] s - the stream
 * @param[out] v - the vector of user data
 */
Stream& operator<<(Stream& s, vector<uint8_t>& v);
} // namespace pels
} // namespace openpower
