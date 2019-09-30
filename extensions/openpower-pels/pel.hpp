#pragma once

#include "additional_data.hpp"
#include "private_header.hpp"
#include "registry.hpp"
#include "user_header.hpp"

#include <memory>
#include <vector>

namespace openpower
{
namespace pels
{

/** @class PEL
 *
 * @brief This class represents a specific event log format referred to as a
 * Platform Event Log.
 *
 * Every field in a PEL are in structures call sections, of which there are
 * several types.  Some sections are required, and some are optional.  In some
 * cases there may be more than one instance of a section type.
 *
 * The only two required sections for every type of PEL are the Private Header
 * section and User Header section, which must be in the first and second
 * positions, respectively.
 *
 * Every section starts with an 8 byte section header, which has the section
 * size and type, among other things.
 *
 * This class represents all sections with objects.
 *
 * The available constructors are:
 * - PEL(std::vector<uint8_t>& data) - build this object out of a fully
 *   formed flattened PEL.
 *
 * - PEL(const openpower::pels::message::Entry& entry,
 *       uint32_t obmcLogID,
 *       uint64_t timestamp,
 *       phosphor::logging::Entry::Level severity)
 *      - build this object from an OpenBMC event log.
 *
 * The data() method allows one to retrieve the PEL as a vector<uint8_t>.  This
 * is the format in which it is stored and transmitted.
 */
class PEL
{
  public:
    PEL() = delete;
    ~PEL() = default;
    PEL(const PEL&) = delete;
    PEL& operator=(const PEL&) = delete;
    PEL(PEL&&) = delete;
    PEL& operator=(PEL&&) = delete;

    /**
     * @brief Constructor
     *
     * Build a PEL from raw data.
     *
     * Note: Neither this nor the following constructor can take a const vector&
     * because the Stream class that is used to read from the vector cannot take
     * a const.  The alternative is to make a copy of the data, but as PELs can
     * be up to 16KB that is undesireable.
     *
     * @param[in] data - The PEL data
     */
    PEL(std::vector<uint8_t>& data);

    /**
     * @brief Constructor
     *
     * Build a PEL from the raw data.
     *
     * @param[in] data - the PEL data
     * @param[in] obmcLogID - the corresponding OpenBMC event log ID
     */
    PEL(std::vector<uint8_t>& data, uint32_t obmcLogID);

    /**
     * @brief Constructor
     *
     * Creates a PEL from an OpenBMC event log and its message
     * registry entry.
     *
     * @param[in] entry - The message registry entry for this error
     * @param[in] obmcLogID - ID of corresponding OpenBMC event log
     * @param[in] timestamp - Timestamp from the event log
     * @param[in] severity - Severity from the event log
     */
    PEL(const openpower::pels::message::Entry& entry, uint32_t obmcLogID,
        uint64_t timestamp, phosphor::logging::Entry::Level severity);

    /**
     * @brief Convenience function to return the log ID field from the
     *        Private Header section.
     *
     * @return uint32_t - the ID
     */
    uint32_t id() const
    {
        return _ph->id();
    }

    /**
     * @brief Convenience function to return the PLID field from the
     *        Private Header section.
     *
     * @return uint32_t - the PLID
     */
    uint32_t plid() const
    {
        return _ph->plid();
    }

    /**
     * @brief Convenience function to return the OpenBMC event log ID field
     * from the Private Header section.
     *
     * @return uint32_t - the OpenBMC event log ID
     */
    uint32_t obmcLogID() const
    {
        return _ph->obmcLogID();
    }

    /**
     * @brief Convenience function to return the commit time field from
     *        the Private Header section.
     *
     * @return BCDTime - the timestamp
     */
    BCDTime commitTime() const
    {
        return _ph->commitTimestamp();
    }

    /**
     * @brief Convenience function to return the create time field from
     *        the Private Header section.
     *
     * @return BCDTime - the timestamp
     */
    BCDTime createTime() const
    {
        return _ph->createTimestamp();
    }

    /**
     * @brief Gives access to the Private Header section class
     *
     * @return std::unique_ptr<PrivateHeader>& the private header
     */
    std::unique_ptr<PrivateHeader>& privateHeader()
    {
        return _ph;
    }

    /**
     * @brief Gives access to the User Header section class
     *
     * @return std::unique_ptr<UserHeader>& the user header
     */
    std::unique_ptr<UserHeader>& userHeader()
    {
        return _uh;
    }

    /**
     * @brief Returns the optional sections, which is everything but
     *        the Private and User Headers.
     *
     * @return const std::vector<std::unique_ptr<Section>>&
     */
    const std::vector<std::unique_ptr<Section>>& optionalSections() const
    {
        return _optionalSections;
    }

    /**
     * @brief Returns the PEL data.
     *
     * @return std::vector<uint8_t> - the raw PEL data
     */
    std::vector<uint8_t> data();

    /**
     * @brief Says if the PEL is valid (the sections are all valid)
     *
     * @return bool - if the PEL is valid
     */
    bool valid() const;

    /**
     * @brief Sets the commit timestamp to the current time
     */
    void setCommitTime();

    /**
     * @brief Sets the error log ID field to a unique ID.
     */
    void assignID();

  private:
    /**
     * @brief Builds the section objects from a PEL data buffer
     *
     * Note: The data parameter cannot be const for the same reasons
     * as listed in the constructor.
     *
     * @param[in] data - The PEL data
     * @param[in] obmcLogID - The OpenBMC event log ID to use for that
     *                        field in the Private Header.
     */
    void populateFromRawData(std::vector<uint8_t>& data, uint32_t obmcLogID);

    /**
     * @brief Flattens the PEL objects into the buffer
     *
     * @param[out] pelBuffer - What the data will be written to
     */
    void flatten(std::vector<uint8_t>& pelBuffer);

    /**
     * @brief The PEL Private Header section
     */
    std::unique_ptr<PrivateHeader> _ph;

    /**
     * @brief The PEL User Header section
     */
    std::unique_ptr<UserHeader> _uh;

    /**
     * @brief Holds all sections by the PH and UH.
     */
    std::vector<std::unique_ptr<Section>> _optionalSections;
};

} // namespace pels
} // namespace openpower
