#pragma once

#include "plugin/plugin_descriptor.hpp"
#include "plugin/utils/artifact.hpp"

#include <string_view>
#include <utility>

namespace phosphor::logging::plugin::cper::raw
{

/**
 * @brief CPERRaw D-Bus interface name.
 */
inline constexpr std::string_view interface =
    "xyz.openbmc_project.Logging.Extension.CPER.Raw";

/**
 * @brief CPER Raw interface properties.
 *
 * Represents the schema-defined properties of the
 * xyz.openbmc_project.Logging.Extension.CPER.Raw interface.
 *
 * This structure serves as the canonical in-memory
 * representation of CPER Raw metadata within the plugin
 * framework.
 */
struct Properties
{
    /**
     * @brief Reference to the persisted CPER artifact.
     *
     * References the persisted raw CPER payload associated
     * with the log entry.
     */
    utils::ArtifactRef artifact;
};

/**
 * @brief CPER Raw plugin creation descriptor.
 *
 * Carries schema-backed CPERRaw metadata extracted from a
 * logging event and serves as the creation input for a
 * runtime CPERRaw plugin instance.
 */
class Descriptor : public phosphor::logging::plugin::Descriptor
{
  public:
    Descriptor() = delete;
    Descriptor(const Descriptor&) = delete;
    Descriptor(Descriptor&&) = default;
    Descriptor& operator=(const Descriptor&) = delete;
    Descriptor& operator=(Descriptor&&) = default;
    ~Descriptor() override = default;

    /**
     * @brief Construct a CPERRaw descriptor.
     *
     * @param[in] properties
     *     CPERRaw interface properties.
     */
    explicit Descriptor(Properties properties) :
        properties_(std::move(properties))
    {}

    /**
     * @brief Return plugin interface identifier.
     *
     * @return Interface name.
     */
    std::string_view interface() const override
    {
        return cper::raw::interface;
    }

    /**
     * @brief Return CPER Raw properties.
     *
     * Consumers should prefer this accessor over
     * field-specific accessors to keep descriptor usage
     * aligned with the underlying schema definition.
     *
     * @return CPER Raw properties.
     */
    const Properties& properties() const
    {
        return properties_;
    }

  private:
    /** Schema-backed CPER Raw properties. */
    Properties properties_;
};

} // namespace phosphor::logging::plugin::cper::raw
