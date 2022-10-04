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
    explicit Policy(const DataInterfaceBase& dataIface) : _dataIface(dataIface)
    {}

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
    explicit LightPath(const DataInterfaceBase& dataIface) : Policy(dataIface)
    {}

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
     * If there are problems looking up any inventory path or LED
     * group, then it will stop and not activate any LEDs at all.
     *
     * @param[in] pel - The PEL
     */
    void activate(const PEL& pel) override;

    /**
     * @brief Returns the location codes for the FRU callouts in the
     *        callouts list that need their LEDs turned on.
     *
     * This is public so it can be tested.
     *
     * @param[in] callouts - The Callout list from a PEL
     *
     * @return std::vector<std::string> - The location codes
     */
    std::vector<std::string> getLocationCodes(
        const std::vector<std::unique_ptr<src::Callout>>& callouts) const;

    /**
     * @brief Function called to check if the code even needs to
     *        bother looking in the callouts to find LEDs to turn on.
     *
     * It will ignore all PELs except for those created by the BMC or
     * hostboot that have the Serviceable action flag set.
     *
     * This is public so it can be tested.
     *
     * @param[in] pel - The PEL
     *
     * @return bool - If the PEL should be ignored or not.
     */
    bool ignore(const PEL& pel) const;

  private:
    /**
     * @brief Returns the inventory D-Bus paths for the passed
     *        in location codes.
     *
     * @param[in] locationCodes - The location codes
     *
     * @return std::vector<std::string> - The inventory D-Bus paths
     */
    std::vector<std::string>
        getInventoryPaths(const std::vector<std::string>& locationCodes) const;

    /**
     * @brief Sets the Functional property on the passed in
     *        inventory paths to false.
     *
     * There is code watching for this that will then turn on
     * any LEDs for that FRU.
     *
     * @param[in] inventoryPaths - The inventory D-Bus paths
     */
    void setNotFunctional(const std::vector<std::string>& inventoryPaths) const;

    /**
     * @brief Sets the critical association on the passed in
     *        inventory paths.
     *
     * @param[in] inventoryPaths - The inventory D-Bus paths
     */
    void createCriticalAssociation(
        const std::vector<std::string>& inventoryPaths) const;

    /**
     * @brief Checks if the callout priority is one that the policy
     *        may turn on an LED for.
     *
     * The priorities it cares about are high, medium, and medium
     * group A.
     *
     * @param[in] priority - The priority value from the PEL
     *
     * @return bool - If LightPath cares about a callout with this
     *                priority.
     */
    bool isRequiredPriority(uint8_t priority) const;

    /**
     * @brief Checks if the callout is either a normal FRU
     *        callout or a symbolic FRU callout with a trusted
     *        location code, which is one of the requirements for
     *        LightPath to turn on an LED.
     *
     * @param[in] - callout - The Callout object
     *
     * @return bool - If the callout is a hardware callout
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
