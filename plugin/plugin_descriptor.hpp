#pragma once

#include <memory>
#include <string_view>
#include <vector>

namespace phosphor::logging::plugin
{

/**
 * @brief Base class for plugin descriptors.
 *
 * A descriptor carries plugin-specific metadata extracted from an
 * event and serves as the input used to construct a runtime plugin.
 *
 * Concrete descriptor implementations define the metadata required
 * by individual plugin interfaces.
 */
class Descriptor
{
  public:
    Descriptor() = default;
    virtual ~Descriptor() = default;

    Descriptor(const Descriptor&) = default;
    Descriptor(Descriptor&&) = default;
    Descriptor& operator=(const Descriptor&) = delete;
    Descriptor& operator=(Descriptor&&) = default;

    /**
     * @brief Get the plugin interface identifier.
     *
     * The interface name is the canonical plugin identity used
     * throughout the framework.
     *
     * @return Plugin interface name.
     */
    virtual std::string_view interface() const = 0;
};

/** Unique ownership of a plugin descriptor. */
using DescriptorPtr = std::unique_ptr<Descriptor>;

/** Collection of plugin descriptors associated with an event. */
using DescriptorList = std::vector<DescriptorPtr>;

} // namespace phosphor::logging::plugin
