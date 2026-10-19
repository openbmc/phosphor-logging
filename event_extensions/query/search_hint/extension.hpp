#pragma once

#include "event_extensions/extension.hpp"
#include "event_extensions/registry.hpp"

#include <nlohmann/json.hpp>
#include <xyz/openbmc_project/Logging/Extension/Query/SearchHint/server.hpp>

#include <string>
#include <string_view>

namespace phosphor::logging::event_extensions::query::search_hint
{

using Interface = sdbusplus::server::xyz::openbmc_project::logging::extension::
    query::SearchHint;

inline constexpr std::string_view interface =
    "xyz.openbmc_project.Logging.Extension.Query.SearchHint";
inline constexpr auto serviceKey = "Service";
inline constexpr auto hintKey = "Hint";

/**
 * @brief Search Hint interface properties.
 *
 * Represents the persisted state exposed through the
 * xyz.openbmc_project.Logging.Extension.Query.SearchHint
 * interface.
 */
struct Properties
{
    std::string service;
    std::string hint;
};

/**
 * @brief Search Hint event extension.
 *
 * Provides the
 * xyz.openbmc_project.Logging.Extension.Query.SearchHint
 * D-Bus interface on a log entry.
 */
class Extension : public event_extensions::Extension, public Interface
{
  public:
    Extension() = delete;
    Extension(const Extension&) = delete;
    Extension(Extension&&) = delete;
    Extension& operator=(const Extension&) = delete;
    Extension& operator=(Extension&&) = delete;
    ~Extension() override = default;

    /**
     * @brief Construct a Search Hint extension.
     *
     * @param[in] bus D-Bus connection.
     * @param[in] objectPath Log entry object path.
     * @param[in] properties Search Hint properties.
     */
    Extension(sdbusplus::bus_t& bus, const sdbusplus::object_path& objectPath,
              Properties properties);

    /**
     * @brief Return implemented interface name.
     *
     * @return Interface name.
     */
    std::string_view interface() const override
    {
        return search_hint::interface;
    }

    /**
     * @brief Serialize extension state.
     *
     * @return Serialized extension state.
     */
    nlohmann::json serialize() const override;

    /**
     * @brief Handle log entry deletion.
     *
     * No extension-specific cleanup is required.
     */
    void onDelete() override {}
};

/**
 * @brief Create a Search Hint extension.
 *
 * Constructs a runtime Search Hint extension from event
 * extension request data. Both Service and Hint must be
 * provided and non-empty.
 *
 * @param[in] context Runtime creation context.
 * @param[in] request Extension request.
 *
 * @return Created extension or nullptr if the request is
 *         invalid.
 */
ExtensionPtr create(const Context& context, const Request& request);

/**
 * @brief Restore a Search Hint extension.
 *
 * Reconstructs a runtime Search Hint extension from
 * serialized extension state.
 *
 * @param[in] context Runtime creation context.
 * @param[in] data Serialized extension state.
 *
 * @return Restored extension or nullptr on failure.
 */
ExtensionPtr restore(const Context& context, const nlohmann::json& data);

/**
 * @brief Register the Search Hint extension.
 *
 * Registers create and restore callbacks with the
 * event extension registry.
 *
 * @param[in,out] registry Event extension registry.
 */
void registerExtension(Registry& registry);

} // namespace phosphor::logging::event_extensions::query::search_hint
