#pragma once

#include "data_interface.hpp"
#include "pel.hpp"

namespace openpower::pels::service_indicators
{

/**
 * @class Policy
 *
 * The base class for service indicator policies.
 */
class Policy
{
  public:
    Policy() = delete;
    virtual ~Policy() = default;
    Policy(const Policy&) = default;
    Policy& operator=(const Policy&) = default;
    Policy(Policy&&) = default;
    Policy& operator=(Policy&&) = default;

    /**
     * @brief Constructor
     *
     * @param[in] dataIface - The DataInterface object
     */
    Policy(const DataInterfaceBase& dataIface) : _dataIface(dataIface)
    {
    }

    /**
     * @brief Pure virtual function for activating service indicators
     *        based on PEL contents.
     *
     * @param[in] pel - The PEL
     */
    virtual void activate(const PEL& pel) = 0;

  protected:
    /**
     * @brief Reference to the DataInterface object
     */
    const DataInterfaceBase& _dataIface;
};

/**
 * @class LightPath
 *
 * This class implements the 'LightPath' IBM policy for
 * activating LEDs.  It has a set of rules to use to choose
 * which callouts inside PELs should have their LEDs asserted,
 * and then activates them by writing the Assert property on
 * LED group D-Bus objects.
 */
class LightPath : public Policy
{
  public:
    LightPath() = delete;
    virtual ~LightPath() = default;
    LightPath(const LightPath&) = default;
    LightPath& operator=(const LightPath&) = default;
    LightPath(LightPath&&) = default;
    LightPath& operator=(LightPath&&) = default;

    /**
     * @brief Constructor
     *
     * @param[in] dataIface - The DataInterface object
     */
    LightPath(const DataInterfaceBase& dataIface) : Policy(dataIface)
    {
    }

    /**
     * @brief Turns on LEDs for certain FRUs called out in the PEL.
     *
     * First it selectively chooses location codes listed in the FRU
     * callout section that it wants to turn on LEDs for.  Next it
     * looks up the inventory D-Bus paths for the FRU represented by
     * those location codes, and then looks for associations to the
     * LED group objects for those inventory paths.  After it has
     * the LED group object, it sets the Asserted property on it.
     *
     * It only does the above for PELs that were created by the BMC
     * or hostboot and have the Serviceable action flag set.
     *
     * If there are problems look any inventory path or LED group,
     * then it will stop and not activate any LEDs at all.
     *
     * @param[in] pel - The PEL
     */
    void activate(const PEL& pel) override;

    /**
     * @brief Description TODO
     */
    std::vector<std::string> getLocationCodes(
        const std::vector<std::unique_ptr<src::Callout>>& callouts) const;

    /**
     * @brief Description TODO
     */
    bool ignore(const PEL& pel) const;

  private:
    /**
     * @brief Description TODO
     */
    std::vector<std::string>
        getLEDGroupPaths(const std::vector<std::string>& locationCodes) const;

    /**
     * @brief Description TODO
     */
    void assertLEDs(const std::vector<std::string>& ledGroups) const;

    /**
     * @brief Description TODO
     */
    bool isRequiredPriority(uint8_t priority) const;

    /**
     * @brief Description TODO
     */
    bool isHardwareCallout(const src::Callout& callout) const;
};

/**
 * @brief Returns the object for the service indicator policy
 *        implemented on the system.
 *
 * @param[in] dataIface - The DataInterface object
 *
 * @return std::unique_ptr<Policy> - The policy object
 *
 */
std::unique_ptr<Policy> getPolicy(const DataInterfaceBase& dataIface);

} // namespace openpower::pels::service_indicators
