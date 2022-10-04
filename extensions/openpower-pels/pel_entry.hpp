#pragma once

#include "manager.hpp"
namespace openpower
{
namespace pels
{

using PELEntryIface = sdbusplus::org::open_power::Logging::PEL::server::Entry;
using PropertiesVariant = PELEntryIface::PropertiesVariant;

class PELEntry : public PELEntryIface
{
  public:
    PELEntry() = delete;
    PELEntry(const PELEntry&) = delete;
    PELEntry& operator=(const PELEntry&) = delete;
    PELEntry(PELEntry&&) = delete;
    PELEntry& operator=(PELEntry&&) = delete;
    virtual ~PELEntry() = default;

    /** @brief Constructor to put an object onto the bus at a dbus path.
     *  @param[in] bus - Bus to attach to.
     *  @param[in] path - Path to attach at.
     *  @param[in] prop - Default property values to be set when this interface
     * is added to the bus.
     *  @param[in] id - obmc id for this instance.
     *  @param[in] repo - Repository pointer to lookup PEL to set appropriate
     * attributes.
     */

    PELEntry(sdbusplus::bus_t& bus, const std::string& path,
             const std::map<std::string, PropertiesVariant>& prop, uint32_t id,
             Repository* repo) :
        PELEntryIface(bus, path.c_str(), prop, true),
        _obmcId(id), _repo(repo)
    {}

    /** @brief Update managementSystemAck flag.
     *  @param[in] value - A true value says HMC acknowledged the PEL.
     *  @returns New property value
     */
    bool managementSystemAck(bool value) override;

    /**
     * @brief Returns OpenBMC event log ID associated with this interface.
     */
    uint32_t getMyId(void) const
    {
        return _obmcId;
    }

  private:
    /**
     * @brief Corresponding OpenBMC event log id of this interface.
     */
    uint32_t _obmcId;

    /**
     * @brief Repository pointer to look for updating PEL fields.
     */
    Repository* _repo;
};

} // namespace pels
} // namespace openpower
