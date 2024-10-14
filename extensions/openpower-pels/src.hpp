#pragma once

#include "additional_data.hpp"
#include "ascii_string.hpp"
#include "callouts.hpp"
#include "data_interface.hpp"
#include "pel_types.hpp"
#include "registry.hpp"
#include "section.hpp"
#include "stream.hpp"

namespace openpower
{
namespace pels
{

constexpr uint8_t srcSectionVersion = 0x01;
constexpr uint8_t srcSectionSubtype = 0x01;
constexpr size_t numSRCHexDataWords = 8;
constexpr uint8_t srcVersion = 0x02;
constexpr uint8_t bmcSRCFormat = 0x55;
constexpr uint8_t primaryBMCPosition = 0x10;
constexpr size_t baseSRCSize = 72;

enum class DetailLevel
{
    message = 0x01,
    json = 0x02
};
/**
 * @class SRC
 *
 * SRC stands for System Reference Code.
 *
 * This class represents the SRC sections in the PEL, of which there are 2:
 * primary SRC and secondary SRC.  These are the same structurally, the
 * difference is that the primary SRC must be the 3rd section in the PEL if
 * present and there is only one of them, and the secondary SRC sections are
 * optional and there can be more than one (by definition, for there to be a
 * secondary SRC, a primary SRC must also exist).
 *
 * This section consists of:
 * - An 8B header (Has the version, flags, hexdata word count, and size fields)
 * - 8 4B words of hex data
 * - An ASCII character string
 * - An optional subsection for Callouts
 */
class SRC : public Section
{
  public:
    enum HeaderFlags
    {
        additionalSections = 0x01,
        powerFaultEvent = 0x02,
        hypDumpInit = 0x04,
        postOPPanel = 0x08,
        i5OSServiceEventBit = 0x10,
        virtualProgressSRC = 0x80
    };

    /**
     * @brief Enums for the error status bits in hex word 5
     *        of BMC SRCs.
     */
    enum class ErrorStatusFlags : uint32_t
    {
        hwCheckstop = 0x80000000,
        terminateFwErr = 0x20000000,
        deconfigured = 0x02000000,
        guarded = 0x01000000
    };

    SRC() = delete;
    ~SRC() = default;
    SRC(const SRC&) = delete;
    SRC& operator=(const SRC&) = delete;
    SRC(SRC&&) = delete;
    SRC& operator=(SRC&&) = delete;

    /**
     * @brief Constructor
     *
     * Fills in this class's data fields from the stream.
     *
     * @param[in] pel - the PEL data stream
     */
    explicit SRC(Stream& pel);

    /**
     * @brief Constructor
     *
     * Creates the section with data from the PEL message registry entry for
     * this error, along with the AdditionalData property contents from the
     * corresponding event log.
     *
     * @param[in] regEntry - The message registry entry for this event log
     * @param[in] additionalData - The AdditionalData properties in this event
     *                             log
     * @param[in] dataIface - The DataInterface object
     */
    SRC(const message::Entry& regEntry, const AdditionalData& additionalData,
        const DataInterfaceBase& dataIface) :
        SRC(regEntry, additionalData, nlohmann::json{}, dataIface)
    {}

    /**
     * @brief Constructor
     *
     * Creates the section with data from the PEL message registry entry for
     * this error, along with the AdditionalData property contents from the
     * corresponding event log, and a JSON array of callouts to add.
     *
     * @param[in] regEntry - The message registry entry for this event log
     * @param[in] additionalData - The AdditionalData properties in this event
     *                             log
     * @param[in] jsonCallouts - The array of JSON callouts, or an empty object.
     * @param[in] dataIface - The DataInterface object
     */
    SRC(const message::Entry& regEntry, const AdditionalData& additionalData,
        const nlohmann::json& jsonCallouts, const DataInterfaceBase& dataIface);

    /**
     * @brief Flatten the section into the stream
     *
     * @param[in] stream - The stream to write to
     */
    void flatten(Stream& stream) const override;

    /**
     * @brief Returns the SRC version, which is a different field
     *        than the version byte in the section header.
     *
     * @return uint8_t
     */
    uint8_t version() const
    {
        return _version;
    }

    /**
     * @brief Returns the flags byte
     *
     * @return uint8_t
     */
    uint8_t flags() const
    {
        return _flags;
    }

    /**
     * @brief Returns the hex data word count.
     *
     * Even though there always 8 words, this returns 9 due to previous
     * SRC version formats.
     *
     * @return uint8_t
     */
    uint8_t hexWordCount() const
    {
        return _wordCount;
    }

    /**
     * @brief Returns the size of the SRC section, not including the header.
     *
     * @return uint16_t
     */
    uint16_t size() const
    {
        return _size;
    }

    /**
     * @brief Returns the 8 hex data words.
     *
     * @return const std::array<uint32_t, numSRCHexDataWords>&
     */
    const std::array<uint32_t, numSRCHexDataWords>& hexwordData() const
    {
        return _hexData;
    }

    /**
     * @brief Returns the ASCII string
     *
     * @return std::string
     */
    std::string asciiString() const
    {
        return _asciiString->get();
    }

    /**
     * @brief Returns the callouts subsection
     *
     * If no callouts, this unique_ptr will be empty
     *
     * @return  const std::unique_ptr<src::Callouts>&
     */
    const std::unique_ptr<src::Callouts>& callouts() const
    {
        return _callouts;
    }

    /**
     * @brief Returns the size of this section when flattened into a PEL
     *
     * @return size_t - the size of the section
     */
    size_t flattenedSize() const
    {
        return _header.size;
    }

    /**
     * @brief Says if this SRC has additional subsections in it
     *
     * Note: The callouts section is the only possible subsection.
     *
     * @return bool
     */
    inline bool hasAdditionalSections() const
    {
        return _flags & additionalSections;
    }

    /**
     * @brief Indicates if this event log is for a power fault.
     *
     * This comes from a field in the message registry for BMC
     * generated PELs.
     *
     * @return bool
     */
    inline bool isPowerFaultEvent() const
    {
        return _flags & powerFaultEvent;
    }

    /**
     * @brief Get the _hexData[] index to use based on the corresponding
     *        SRC word number.
     *
     * Converts the specification nomenclature to this data structure.
     * See the _hexData documentation below for more information.
     *
     * @param[in] wordNum - The SRC word number, as defined by the spec.
     *
     * @return size_t The corresponding index into _hexData.
     */
    inline size_t getWordIndexFromWordNum(size_t wordNum) const
    {
        assert(wordNum >= 2 && wordNum <= 9);
        return wordNum - 2;
    }

    /**
     * @brief Get section in JSON.
     * @param[in] registry - Registry object reference
     * @param[in] plugins - Vector of strings of plugins found in filesystem
     * @param[in] creatorID - Creator Subsystem ID from Private Header
     * @return std::optional<std::string> - SRC section's JSON
     */
    std::optional<std::string> getJSON(message::Registry& registry,
                                       const std::vector<std::string>& plugins,
                                       uint8_t creatorID) const override;

    /**
     * @brief Get error details based on refcode and hexwords
     * @param[in] registry - Registry object
     * @param[in] type - detail level enum value : single message or full json
     * @param[in] toCache - boolean to cache registry in memory, default=false
     * @return std::optional<std::string> - Error details
     */
    std::optional<std::string>
        getErrorDetails(message::Registry& registry, DetailLevel type,
                        bool toCache = false) const;

    /**
     * @brief Says if this SRC was created by the BMC (i.e. this code).
     *
     * @return bool - If created by the BMC or not
     */
    bool isBMCSRC() const;

    /**
     * @brief Says if this SRC was created by Hostboot
     *
     * @return bool - If created by Hostboot or not
     */
    bool isHostbootSRC() const;

    /**
     * @brief Set the terminate bit in hex data word 3.
     */
    void setTerminateBit()
    {
        setErrorStatusFlag(ErrorStatusFlags::terminateFwErr);
    }

    /**
     * @brief Get the SRC structure to pass on to the boot progress dbus
     * interface.
     *
     * @return std::vector<uint8_t> - SRC struct data
     */
    std::vector<uint8_t> getSrcStruct();

    /**
     * @brief Extracts the first 8 characters of the ASCII String field
     *        from the raw progress SRC and converts it to a uint32_t.
     *
     * @param[in] rawProgressSRC - The progress SRC bytes
     *
     * @return uint32_t - The code, like 0xCC0099EE from "CC0099EE"
     */
    static uint32_t getProgressCode(std::vector<uint8_t>& rawProgressSRC);

    /**
     * @brief Return the value of the passed in error status flag.
     *
     * @param[in] flag - The flag
     *
     * @return bool - If the flag is set.
     */
    bool getErrorStatusFlag(ErrorStatusFlags flag) const
    {
        return _hexData[3] & static_cast<uint32_t>(flag);
    }

    /**
     * @brief Clears an error status flag in the SRC.
     *
     * @param[in] flag - The flag to set
     */
    void clearErrorStatusFlag(ErrorStatusFlags flag)
    {
        _hexData[3] &= ~static_cast<uint32_t>(flag);
    }

  private:
    /**
     * @brief Fills in the user defined hex words from the
     *        AdditionalData fields.
     *
     * When creating this section from a message registry entry,
     * that entry has a field that says which AdditionalData property
     * fields to use to fill in the user defined hex data words 6-9
     * (which correspond to hexData words 4-7).
     *
     * For example, given that AdditionalData is a map of string keys
     * to string values, find the AdditionalData value for AdditionalData
     * key X, convert it to a uint32_t, and save it in user data word Y.
     *
     * @param[in] regEntry - The message registry entry for the error
     * @param[in] additionalData - The AdditionalData map
     */
    void setUserDefinedHexWords(const message::Entry& regEntry,
                                const AdditionalData& additionalData);
    /**
     * @brief Fills in the object from the stream data
     *
     * @param[in] stream - The stream to read from
     */
    void unflatten(Stream& stream);

    /**
     * @brief Says if the word number is in the range of user defined words.
     *
     * This is only used for BMC generated SRCs, where words 6 - 9 are the
     * user defined ones, meaning that setUserDefinedHexWords() will be
     * used to fill them in based on the contents of the OpenBMC event log.
     *
     * @param[in] wordNum - The SRC word number, as defined by the spec.
     *
     * @return bool - If this word number can be filled in by the creator.
     */
    inline bool isUserDefinedWord(size_t wordNum) const
    {
        return (wordNum >= 6) && (wordNum <= 9);
    }

    /**
     * @brief Sets the SRC format byte in the hex word data.
     */
    inline void setBMCFormat()
    {
        _hexData[0] |= bmcSRCFormat;
    }

    /**
     * @brief Sets the hex word field that specifies which BMC
     *        (primary vs backup) created the error.
     *
     * Can be hardcoded until there are systems with redundant BMCs.
     */
    inline void setBMCPosition()
    {
        _hexData[1] |= primaryBMCPosition;
    }

    /**
     * @brief Sets the motherboard CCIN hex word field
     *
     * @param[in] dataIface - The DataInterface object
     */
    void setMotherboardCCIN(const DataInterfaceBase& dataIface);

    /**
     * @brief Sets the progress code hex word field
     *
     * @param[in] dataIface - The DataInterface object
     */
    void setProgressCode(const DataInterfaceBase& dataIface);

    /**
     * @brief Sets an error status bit in the SRC.
     *
     * @param[in] flag - The flag to set
     */
    void setErrorStatusFlag(ErrorStatusFlags flag)
    {
        _hexData[3] |= static_cast<uint32_t>(flag);
    }

    /**
     * @brief Validates the section contents
     *
     * Updates _valid (in Section) with the results.
     */
    void validate() override;

    /**
     * @brief Get error description from message registry
     * @param[in] regEntry - The message registry entry for the error
     * @return std::optional<std::string> - Error message
     */
    std::optional<std::string>
        getErrorMessage(const message::Entry& regEntry) const;

    /**
     * @brief Get Callout info in JSON
     * @return std::optional<std::string> - Callout details
     */
    std::optional<std::string> getCallouts() const;

    /**
     * @brief Checks the AdditionalData property and the message registry
     *        JSON and adds any necessary callouts.
     *
     * The callout sources are the AdditionalData event log property
     * and the message registry JSON.
     *
     * @param[in] regEntry - The message registry entry for the error
     * @param[in] additionalData - The AdditionalData values
     * @param[in] jsonCallouts - The array of JSON callouts, or an empty object
     * @param[in] dataIface - The DataInterface object
     */
    void addCallouts(const message::Entry& regEntry,
                     const AdditionalData& additionalData,
                     const nlohmann::json& jsonCallouts,
                     const DataInterfaceBase& dataIface);

    /**
     * @brief Adds a FRU callout based on an inventory path
     *
     * @param[in] inventoryPath - The inventory item to call out
     * @param[in] priority - An optional priority (uses high if nullopt)
     * @param[in] locationCode - The expanded location code (or look it up)
     * @param[in] dataIface - The DataInterface object
     * @param[in] mrus - The MRUs to add to the callout
     */
    void addInventoryCallout(
        const std::string& inventoryPath,
        const std::optional<CalloutPriority>& priority,
        const std::optional<std::string>& locationCode,
        const DataInterfaceBase& dataIface,
        const std::vector<src::MRU::MRUCallout>& mrus = {});

    /**
     * @brief Returns the callouts to use from the registry entry.
     *
     * @param[in] regEntry - The message registry entry for the error
     * @param[in] additionalData - The AdditionalData property
     * @param[in] dataIface - The DataInterface object
     */
    std::vector<message::RegistryCallout> getRegistryCallouts(
        const message::Entry& regEntry, const AdditionalData& additionalData,
        const DataInterfaceBase& dataIface);

    /**
     * @brief Adds the FRU callouts from the list of registry callouts
     *        passed in to the SRC.
     *
     * The last parameter is used only in a special case when the first
     * callout is a symbolic FRU with a trusted location code.  See the
     * addRegistryCallout documentation.
     *
     * @param[in] callouts - The message registry callouts to add
     * @param[in] dataIface - The DataInterface object
     * @param[in] trustedSymbolicFRUInvPath - The optional inventory path used
     *                                        in the symbolic FRU case.
     */
    void addRegistryCallouts(
        const std::vector<message::RegistryCallout>& callouts,
        const DataInterfaceBase& dataIface,
        std::optional<std::string> trustedSymbolicFRUInvPath);

    /**
     * @brief Adds a single FRU callout from the message registry.
     *
     * If the last parameter is filled in, and the registry callout is a
     * symbolic FRU callout with a trusted location code, and it has the
     * 'useInventoryLocCode' member set to true, then the location code of
     * that inventory item will be what is used for that trusted location code.
     *
     * @param[in] callout - The registry callout structure
     * @param[in] dataIface - The DataInterface object
     * @param[in] trustedSymbolicFRUInvPath - The optional inventory path used
     *                                        in the symbolic FRU case.
     */
    void addRegistryCallout(
        const message::RegistryCallout& callout,
        const DataInterfaceBase& dataIface,
        const std::optional<std::string>& trustedSymbolicFRUInvPath);

    /**
     * @brief Creates the Callouts object _callouts
     *        so that callouts can be added to it.
     */
    void createCalloutsObject()
    {
        if (!_callouts)
        {
            _callouts = std::make_unique<src::Callouts>();
            _flags |= additionalSections;
        }
    }

    /**
     * @brief Adds any FRU callouts based on a device path in the
     *        AdditionalData parameter.
     *
     * @param[in] additionalData - The AdditionalData values
     * @param[in] dataIface - The DataInterface object
     */
    void addDevicePathCallouts(const AdditionalData& additionalData,
                               const DataInterfaceBase& dataIface);

    /**
     * @brief Adds any FRU callouts specified in the incoming JSON.
     *
     * @param[in] jsonCallouts - The JSON array of callouts
     * @param[in] dataIface - The DataInterface object
     */
    void addJSONCallouts(const nlohmann::json& jsonCallouts,
                         const DataInterfaceBase& dataIface);

    /**
     * @brief Adds a single callout based on the JSON
     *
     * @param[in] jsonCallouts - A single callout entry
     * @param[in] dataIface - The DataInterface object
     */
    void addJSONCallout(const nlohmann::json& jsonCallout,
                        const DataInterfaceBase& dataIface);

    /**
     * @brief Extracts a CalloutPriority value from the json
     *        using the 'Priority' key.
     *
     * @param[in] json - A JSON object that contains the priority key
     *
     * @return CalloutPriority - The priority value
     */
    CalloutPriority getPriorityFromJSON(const nlohmann::json& json);

    /**
     * @brief Exracts MRU values and their priorities from the
     *        input JSON array.
     *
     * @param[in] mruJSON - The JSON array
     */
    std::vector<src::MRU::MRUCallout>
        getMRUsFromJSON(const nlohmann::json& mruJSON);

    /**
     * @brief The SRC version field
     */
    uint8_t _version;

    /**
     * @brief The SRC flags field
     */
    uint8_t _flags;

    /**
     * @brief A byte of reserved data after the flags field
     */
    uint8_t _reserved1B;

    /**
     * @brief The hex data word count.
     *
     * To be compatible with previous versions of SRCs, this is
     * number of hex words (8) + 1 = 9.
     */
    uint8_t _wordCount;

    /**
     * @brief Two bytes of reserved data after the hex word count
     */
    uint16_t _reserved2B;

    /**
     * @brief The total size of the SRC section, not including the section
     *        header.
     */
    uint16_t _size;

    /**
     * @brief The SRC 'hex words'.
     *
     * In the spec these are referred to as SRC words 2 - 9 as words 0 and 1
     * are filled by the 8 bytes of fields from above.
     */
    std::array<uint32_t, numSRCHexDataWords> _hexData;

    /**
     * @brief The 32 byte ASCII character string of the SRC
     *
     * It is padded with spaces to fill the 32 bytes.
     * An example is:
     * "BD8D1234                        "
     *
     * That first word is what is commonly referred to as the refcode, and
     * sometimes also called an SRC.
     */
    std::unique_ptr<src::AsciiString> _asciiString;

    /**
     * @brief The callouts subsection.
     *
     * Optional and only created if there are callouts.
     */
    std::unique_ptr<src::Callouts> _callouts;
};

} // namespace pels
} // namespace openpower
