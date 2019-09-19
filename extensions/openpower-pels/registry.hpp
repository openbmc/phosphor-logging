#pragma once
#include <filesystem>
#include <map>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace openpower
{
namespace pels
{
namespace message
{

constexpr auto registryFileName = "message_registry.json";

/**
 * @brief Represents a message registry entry, which is used for creating a
 *        PEL from an OpenBMC event log.
 */
struct Entry
{
    /**
     * @brief The error name, like "xyz.openbmc_project.Error.Foo".
     */
    std::string name;

    /**
     * @brief The PEL subsystem field.
     */
    uint8_t subsystem;

    /**
     * @brief The optional PEL severity field.  If not specified, the PEL
     *        will use the severity of the OpenBMC event log.
     */
    std::optional<uint8_t> severity;

    /**
     * @brief The optional severity field to use when in manufacturing tolerance
     *        mode.
     */
    std::optional<uint8_t> mfgSeverity;

    /**
     * @brief The PEL action flags field.
     */
    uint16_t actionFlags;

    /**
     * @brief  The optional action flags to use instead when in manufacturing
     * tolerance mode.
     */
    std::optional<uint16_t> mfgActionFlags;

    /**
     * @brief The PEL event type field.  If not specified, higher level code
     *        will decide the value.
     */
    std::optional<uint8_t> eventType;

    /**
     * @brief The PEL event scope field.  If not specified, higher level code
     *        will decide the value.
     */
    std::optional<uint8_t> eventScope;

    // TODO: SRC related fields
};

/**
 * @class Registry
 *
 * This class wraps the message registry JSON data and allows one to find
 * the message registry entry pertaining to the error name.
 *
 * So that new registry files can easily be tested, the code will look for
 * /etc/phosphor-logging/message_registry.json before looking for the real
 * path.
 */
class Registry
{
  public:
    Registry() = delete;
    ~Registry() = default;
    Registry(const Registry&) = default;
    Registry& operator=(const Registry&) = default;
    Registry(Registry&&) = default;
    Registry& operator=(Registry&&) = default;

    /**
     * @brief Constructor
     * @param[in] registryFile - The path to the file.
     */
    explicit Registry(const std::filesystem::path& registryFile) :
        _registryFile(registryFile)
    {
    }

    /**
     * @brief Find a registry entry based on its error name.
     *
     * This function does do some basic sanity checking on the JSON contents,
     * but there is also an external program that enforces a schema on the
     * registry JSON that should catch all of these problems ahead of time.
     *
     * @param[in] name - The error name, like xyz.openbmc_project.Error.Foo
     *
     * @return optional<Entry> A filled in message registry structure if
     *                         found, otherwise an empty optional object.
     */
    std::optional<Entry> lookup(const std::string& name);

  private:
    /**
     * @brief The path to the registry JSON file.
     */
    std::filesystem::path _registryFile;
};

namespace helper
{

/**
 * @brief A helper function to get the PEL subsystem value based on
 *        the registry subsystem name.
 *
 * @param[in] subsystemName - The registry name for the subsystem
 *
 * @return uint8_t The PEL subsystem value
 */
uint8_t getSubsystem(const std::string& subsystemName);

/**
 * @brief A helper function to get the PEL severity value based on
 *        the registry severity name.
 *
 * @param[in] severityName - The registry name for the severity
 *
 * @return uint8_t The PEL severity value
 */
uint8_t getSeverity(const std::string& severityName);

/**
 * @brief A helper function to get the action flags value based on
 *        the action flag names used in the registry.
 *
 * @param[in] flags - The list of flag names from the registry.
 *
 * @return uint16_t - The bitfield of flags used in the PEL.
 */
uint16_t getActionFlags(const std::vector<std::string>& flags);

/**
 * @brief A helper function to get the PEL event type value based on
 *        the registry event type name.
 *
 * @param[in] eventTypeName - The registry name for the event type
 *
 * @return uint8_t The PEL event type value
 */
uint8_t getEventType(const std::string& eventTypeName);

/**
 * @brief A helper function to get the PEL event scope value based on
 *        the registry event scope name.
 *
 * @param[in] eventScopeName - The registry name for the event scope
 *
 * @return uint8_t The PEL event scope value
 */
uint8_t getEventScope(const std::string& eventScopeName);

} // namespace helper

} // namespace message

} // namespace pels
} // namespace openpower
