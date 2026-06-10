#pragma once

#include <sdbusplus/async.hpp>
#include <sdbusplus/exception.hpp>

#include <initializer_list>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace lg2
{

using AdditionalData_t = std::map<std::string, std::string>;

/** @brief Well-known additional-data keys.
 *
 *  These are the conventional keys understood across phosphor-logging.
 *  Use AdditionalData::set(AdField, ...) to populate them in a typo-proof,
 *  discoverable way. Arbitrary client-defined keys remain supported via the
 *  string overload of set().
 */
enum class AdField
{
    calloutInventoryPath,
    calloutDevicePath,
};

/** @brief Map a well-known field to its on-the-wire key string. */
constexpr std::string_view to_string(AdField field)
{
    switch (field)
    {
        case AdField::calloutInventoryPath:
            return "CALLOUT_INVENTORY_PATH";
        case AdField::calloutDevicePath:
            return "CALLOUT_DEVICE_PATH";
    }
    return "";
}

/** @brief Builder/holder for an event's additional data.
 *
 *  Wraps an AdditionalData_t (std::map<string, string>) and provides
 *  type-checked setters for well-known keys (AdField) while still allowing
 *  arbitrary client-defined keys.
 *
 *  It is implicitly constructible from a map or a brace-initializer list of
 *  key/value pairs, so existing call sites that pass either continue to work
 *  unchanged:
 *
 *      lg2::commit(event, std::nullopt, someMap);
 *      lg2::commit(event, std::nullopt, {{"KEY", "VALUE"}});
 *      lg2::commit(event, std::nullopt,
 *                  AdditionalData{}.set(AdField::calloutInventoryPath, path)
 *                                  .set("VENDOR_KEY", "foo"));
 */
class AdditionalData
{
  public:
    AdditionalData() = default;

    /** @brief Construct from an existing map (copy). */
    AdditionalData(const AdditionalData_t& data) : data(data) {}

    /** @brief Construct from an existing map (move). */
    AdditionalData(AdditionalData_t&& data) : data(std::move(data)) {}

    /** @brief Construct from a brace-init list of key/value pairs. */
    AdditionalData(std::initializer_list<AdditionalData_t::value_type> init) :
        data(init)
    {}

    /** @brief Set a well-known field. */
    AdditionalData& set(AdField field, std::string value)
    {
        data[std::string(to_string(field))] = std::move(value);
        return *this;
    }

    /** @brief Set an arbitrary client-defined key. */
    AdditionalData& set(std::string key, std::string value)
    {
        data[std::move(key)] = std::move(value);
        return *this;
    }

    /** @brief Access the underlying map. */
    const AdditionalData_t& map() const& { return data; }

    /** @brief Move out the underlying map. */
    AdditionalData_t map() && { return std::move(data); }

  private:
    AdditionalData_t data;
};

/** Commit a generated event/error.
 *
 *  @param e - The event to commit.
 *  @param overrideLevel - Optional severity level override.
 *  @param additionalData - Optional additional key-value data to include.
 *  @return The object path of the resulting event.
 *
 *  Note: Similar to elog(), this will use the default dbus connection to
 *  perform the operation.
 */
auto commit(sdbusplus::exception::generated_event_base&& e,
            std::optional<int> overrideLevel = std::nullopt,
            AdditionalData additionalData = {}) -> sdbusplus::object_path;

/** Resolve an existing event/error.
 *
 *  @param logPath - The object path of the event to resolve.
 *  @return None.
 *
 *  Note: Similar to elog(), this will use the default dbus connection to
 *  perform the operation.
 */
void resolve(const sdbusplus::object_path& logPath);

/** Commit a generated event/error (using async context).
 *
 *  @param ctx - The async context to use.
 *  @param e - The event to commit.
 *  @param overrideLevel - Optional severity level override.
 *  @param additionalData - Optional additional key-value data to include.
 *  @return The object path of the resulting event.
 */
auto commit(sdbusplus::async::context& ctx,
            sdbusplus::exception::generated_event_base&& e,
            std::optional<int> overrideLevel = std::nullopt,
            AdditionalData additionalData = {})
    -> sdbusplus::async::task<sdbusplus::object_path>;

/** Resolve an existing event/error (using async context).
 *
 *  @param ctx - The async context to use.
 *  @param logPath - The object path of the event to resolve.
 *  @return None
 */
auto resolve(sdbusplus::async::context& ctx,
             const sdbusplus::object_path& logPath) -> sdbusplus::async::task<>;

} // namespace lg2
