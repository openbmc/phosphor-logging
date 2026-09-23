#pragma once

#include <nlohmann/json.hpp>
#include <xyz/openbmc_project/Logging/CPER/Types/common.hpp>
#include <xyz/openbmc_project/Logging/Extension/CPER/Processed/common.hpp>

#include <cstdint>
#include <functional>
#include <span>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace phosphor::logging::cper::v1
{

namespace dbus
{
using Types = sdbusplus::common::xyz::openbmc_project::logging::cper::Types;
using Processed = sdbusplus::common::xyz::openbmc_project::logging::extension::
    cper::Processed;
} // namespace dbus

/** CPER content type: full record vs single section. */
using Type = dbus::Types::ContentType;

/** The D-Bus `Oem` property map: key to JSON-encoded string value. */
using Oem = dbus::Processed::oem_t::value_type;

/** Section-type GUID string, as in `NotificationType`/`SectionType`. */
using Guid = dbus::Processed::notification_type_t::value_type;

static_assert(std::is_same_v<Guid, dbus::Processed::section_type_t::value_type>,
              "NotificationType and SectionType GUIDs must match");

/** Single new `Oem` entry contributed by a plugin. */
struct Entry
{
    /** New `Oem` map key, e.g. `"MyVendor"`. Must not be `"OpenBMC"`. */
    std::string key;
    /** Structured payload; the Decoder dumps it to a string. */
    nlohmann::json value;
};

/** Plugin decode function.
 *
 * Called once per record when at least one of the plugin's GUIDs matches
 * a section in the record.
 *
 * @param[in] type - Whether the input is a full record or a section.
 * @param[in] raw - The full original bytes, valid for the call only.
 * @param[in] base - The `Oem` map built by libcper processing.
 * @param[in] guids - The subset of this plugin's GUIDs found in the record.
 * @return The new `Oem` entry to merge.  An empty key indicates none.
 * @throw Any exception to skip this plugin; the Decoder logs and continues.
 */
using Handler =
    std::function<Entry(Type type, std::span<const std::uint8_t> raw,
                        const Oem& base, std::span<const Guid> guids)>;

/** Register a handler for one section-type GUID. */
void add(Guid guid, Handler handler);

/** Register one handler for several section-type GUIDs. */
void add(std::vector<Guid> guids, Handler handler);

/** RAII static registration.
 *
 * Example:
 * @code
 * #include <phosphor-logging/cper.hpp>
 *
 * namespace v1 = phosphor::logging::cper::v1;
 *
 * namespace
 * {
 * const v1::Register reg{
 *     "12345678-1234-5678-1234-567812345678",
 *     [](v1::Type, std::span<const std::uint8_t> raw, const v1::Oem&,
 *        std::span<const v1::Guid>) -> v1::Entry {
 *         return {.key = "MyVendor", .value = {{"field", 1}}};
 *     }};
 * }
 * @endcode
 */
class Register
{
  public:
    Register(Guid guid, Handler handler)
    {
        add(std::move(guid), std::move(handler));
    }

    Register(std::vector<Guid> guids, Handler handler)
    {
        add(std::move(guids), std::move(handler));
    }

    Register(const Register&) = delete;
    Register& operator=(const Register&) = delete;
    Register(Register&&) = delete;
    Register& operator=(Register&&) = delete;
    ~Register() = default;
};

} // namespace phosphor::logging::cper::v1
