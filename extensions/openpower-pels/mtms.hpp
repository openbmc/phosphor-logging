#pragma once

#include "stream.hpp"

#include <string>

namespace openpower
{
namespace pels
{

/** @class MTMS
 *
 * (M)achine (T)ype-(M)odel (S)erialNumber
 *
 * Represents the PEL's Machine Type / Model / Serial Number
 * structure, which shows up in multiple places in a PEL.
 *
 * It holds 8 bytes for the Type+Model, and 12 for the SN.
 * Unused bytes are set to 0s.
 *
 * The type and model are combined into a single field.
 */
class MTMS
{
  public:
    MTMS(const MTMS&) = default;
    MTMS& operator=(const MTMS&) = default;
    MTMS(MTMS&&) = default;
    MTMS& operator=(MTMS&&) = default;
    ~MTMS() = default;

    enum
    {
        mtmSize = 8,
        snSize = 12
    };

    /**
     * @brief Constructor
     */
    MTMS();

    /**
     * @brief Constructor
     *
     * @param[in] typeModel - The machine type+model
     * @param[in] serialNumber - The machine serial number
     */
    MTMS(const std::string& typeModel, const std::string& serialNumber);

    /**
     * @brief Constructor
     *
     * Fills in this class's data fields from the stream.
     *
     * @param[in] pel - the PEL data stream
     */
    MTMS(Stream& stream);

    /**
     * @brief Returns the machine type/model value
     *
     * @return std::array<uint8_t, mtmSize>&  - The TM value
     */
    std::array<uint8_t, mtmSize>& machineTypeAndModel()
    {
        return _machineTypeAndModel;
    }

    /**
     * @brief Returns the machine serial number value
     *
     * @return std::array<uint8_t, snSize>& - The SN value
     */
    std::array<uint8_t, snSize>& machineSerialNumber()
    {
        return _serialNumber;
    }

    /**
     * @brief Returns the size of the data when flattened
     *
     * @return size_t - The size of the data
     */
    static constexpr size_t flattenedSize()
    {
        return mtmSize + snSize;
    }

  private:
    /**
     * @brief The type+model value
     *
     *     Of the form TTTT-MMM where:
     *        TTTT = machine type
     *        MMM = machine model
     */
    std::array<uint8_t, mtmSize> _machineTypeAndModel;

    /**
     * @brief Machine Serial Number
     */
    std::array<uint8_t, snSize> _serialNumber;
};

/**
 * @brief Stream extraction operator for MTMS
 *
 * @param[in] s - the stream
 * @param[out] mtms - the MTMS object
 *
 * @return Stream&
 */
Stream& operator>>(Stream& s, MTMS& mtms);

/**
 * @brief Stream insertion operator for MTMS
 *
 * @param[out] s - the stream
 * @param[in] mtms - the MTMS object
 *
 * @return Stream&
 */
Stream& operator<<(Stream& s, MTMS& mtms);

} // namespace pels
} // namespace openpower
