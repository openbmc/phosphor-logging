#pragma once

#include "plugin/plugin_type.hpp"

#include <memory>
#include <vector>

namespace phosphor::logging::plugin
{

/**
 * @brief Base class for plugin descriptors.
 *
 * A descriptor carries plugin-specific metadata extracted from an
 * event and serves as the input used to construct or invoke a
 * logging plugin.
 *
 * Concrete descriptor implementations define the metadata required
 * by individual plugin types.
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
     * Get the descriptor plugin type.
     *
     * @return Plugin type represented by this descriptor.
     */
    virtual Type type() const = 0;
};

/** Unique ownership of a plugin descriptor. */
using DescriptorPtr = std::unique_ptr<Descriptor>;

/** Collection of plugin descriptors associated with an event. */
using DescriptorList = std::vector<DescriptorPtr>;

} // namespace phosphor::logging::plugin
