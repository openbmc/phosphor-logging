#pragma once

#include <sdbusplus/bus.hpp>

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace phosphor::logging::event_extensions
{

/**
 * @brief Runtime context used during extension creation.
 *
 * Provides resources required when associating
 * extensions with a log entry.
 */
struct Context
{
    /** D-Bus connection. */
    sdbusplus::bus_t& bus;

    /** Log entry object path. */
    const std::string& objectPath;
};

/**
 * @brief Base class for runtime event extensions.
 *
 * Extension instances are owned by a log entry and
 * exist for the lifetime of that entry.
 *
 * Event extensions augment log entries with additional
 * D-Bus interfaces and extension-specific behavior.
 */
class Extension
{
  public:
    virtual ~Extension() = default;

    /**
     * @brief Return implemented D-Bus interface name.
     *
     * @return Interface name.
     */
    virtual std::string_view interface() const = 0;

    /**
     * @brief Invoked before the owning log entry is deleted.
     *
     * Extensions may override this method to release
     * extension-owned resources.
     */
    virtual void onDelete() {}
};

using ExtensionPtr = std::unique_ptr<Extension>;
using ExtensionList = std::vector<ExtensionPtr>;

} // namespace phosphor::logging::event_extensions
