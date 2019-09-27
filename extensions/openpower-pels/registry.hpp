#pragma once
#include <filesystem>
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
 * @brief Represents the SRC related fields in the message registry.
 *        It is part of the 'Entry' structure that will be filled in when
 *        an error is looked up in the registry.
 *
 * If a field is wrapped by std::optional, it means the field is
 * optional in the JSON and higher level code knows how to handle it.
 */
struct SRC
{
    /**
     * @brief SRC type - The first byte of the ASCII string
     */
    uint8_t type;

    /**
     * @brief The SRC reason code (2nd half of 4B 'ASCII string' word)
     */
    uint16_t reasonCode;

    /**
     * @brief Specifies if the SRC represents a power fault.  Optional.
     */
    std::optional<bool> powerFault;

    /**
     * @brief An optional vector of SRC hexword numbers that should be used
     *        along with the SRC ASCII string to build the Symptom ID, which
     *        is a field in the Extended Header section.
     */
    using WordNum = size_t;
    std::optional<std::vector<WordNum>> symptomID;

    /**
     * @brief Which AdditionalData fields to use to fill in the user defined
     *        SRC hexwords.
     *
     * For example, if the AdditionalData event log property contained
     * "CHIPNUM=42" and this map contained {6, CHIPNUM}, then the code
     * would put 42 into SRC hexword 6.
     */
    using AdditionalDataField = std::string;
    std::optional<std::map<WordNum, AdditionalDataField>> hexwordADFields;

    SRC() : type(0), reasonCode(0)
    {
    }
};

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
     * @brief The component ID of the PEL creator.
     */
    uint16_t componentID;

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

    /**
     * The SRC related fields.
     */
    SRC src;
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

/**
 * @brief Reads the "ReasonCode" field out of JSON and converts the string value
 *        such as "0x5555" to a uint16 like 0x5555.
 *
 * @param[in] src - The message registry SRC dictionary to read from
 * @param[in] name - The error name, to use in a trace if things go awry.
 *
 * @return uint16_t - The reason code
 */
uint16_t getSRCReasonCode(const nlohmann::json& src, const std::string& name);

/**
 * @brief Reads the "Type" field out of JSON and converts it to the SRC::Type
 *        value.
 *
 * @param[in] src - The message registry SRC dictionary to read from
 * @param[in] name - The error name, to use in a trace if things go awry.
 *
 * @return uint8_t - The SRC type value, like 0x11
 */
uint8_t getSRCType(const nlohmann::json& src, const std::string& name);

/**
 * @brief Reads the "Words6To9" field out of JSON and converts it to a map
 *        of the SRC word number to the AdditionalData property field used
 *        to fill it in with.
 *
 * @param[in] src - The message registry SRC dictionary to read from
 * @param[in] name - The error name, to use in a trace if things go awry.
 *
 * @return std::optional<std::map<SRC::WordNum, SRC::AdditionalDataField>>
 */
std::optional<std::map<SRC::WordNum, SRC::AdditionalDataField>>
    getSRCHexwordFields(const nlohmann::json& src, const std::string& name);

/**
 * @brief Reads the "SymptomIDFields" field out of JSON and converts it to
 *        a vector of SRC word numbers.
 *
 * @param[in] src - The message registry SRC dictionary to read from
 * @param[in] name - The error name, to use in a trace if things go awry.
 *
 * @return std::optional<std::vector<SRC::WordNum>>
 */
std::optional<std::vector<SRC::WordNum>>
    getSRCSymptomIDFields(const nlohmann::json& src, const std::string& name);

/**
 * @brief Reads the "ComponentID" field out of JSON and converts it to a
 *        uint16_t like 0xFF00.
 *
 * The ComponentID JSON field is only required if the SRC type isn't a BD
 * BMC SRC, because for those SRCs it can be inferred from the upper byte
 * of the SRC reasoncode.
 *
 * @param[in] srcType - The SRC type
 * @param[in] reasonCode - The SRC reason code
 * @param[in] pelEntry - The PEL entry JSON
 * @param[in] name - The error name, to use in a trace if things go awry.
 *
 * @return uin16_t - The component ID, like 0xFF00
 */
uint16_t getComponentID(uint8_t srcType, uint16_t reasonCode,
                        const nlohmann::json& pelEntry,
                        const std::string& name);

} // namespace helper

} // namespace message

} // namespace pels
} // namespace openpower
