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
 * @class userData
 *
 * This represents the User Data section in a PEL.
 *
 * The Section base class handles the section header structure that every
 * PEL section has at offset zero.
 *
 * The fields in this class directly correspond to the order and sizes of
 * the fields in the section.
 */
class userData : public Section
{
  public:
    userData() = delete;
    ~userData() = default;
    userData(const userData&) = default;
    userData& operator=(const userData&) = default;
    userData(userData&&) = default;
    userData& operator=(userData&&) = default;

    /**
     * @brief Constructor
     *
     * Fills in this class's data fields from the stream.
     *
     * @param[in] pel - the PEL data stream
     */
    explicit userData(Stream& pel);

    /**
     * @brief Returns the size of this section when flattened into a PEL
     *
     * @return size_t - the size of the section
     */
    static constexpr size_t flattenedSize()
    {
        return Section::flattenedSize() + sizeof(#TODO);
    }

    friend Stream& operator>>(Stream& s, userData& ud);
    friend Stream& operator<<(Stream& s, userData& ud);

  private:
    /**
     * @brief Validates the section contents
     *
     * Updates _valid (in Section) with the results.
     */
    void validate() override;

    /**
     * @brief user data - this can be anything
     * so I'm going with auto
     */
    auto _data;
};

/**
 * @brief Stream extraction operator for the userData
 *
 * @param[in] s - the stream
 * @param[out] ud - the userData object
 */
Stream& operator>>(Stream& s, userData& ud);

/**
 * @brief Stream insertion operator for the userData
 *
 * @param[out] s - the stream
 * @param[in] ud - the userData object
 */
Stream& operator<<(Stream& s, userData& ud);

} // namespace pels
} // namespace openpower
