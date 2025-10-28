#pragma once

#include <sdbusplus/bus.hpp>

#include <map>
#include <optional>
#include <string>
#include <vector>

namespace phosphor::logging::util
{

/**
 * @brief Return a value found in the /etc/os-release file
 *
 * @param[in] key - The key name, like "VERSION"
 *
 * @return std::optional<std::string> - The value
 */
std::optional<std::string> getOSReleaseValue(const std::string& key);

/**
 * @brief Synchronize unwritten journal messages to disk.
 * @details This is the same implementation as the systemd command
 *          "journalctl --sync".
 */
void journalSync();

/**
 * @brief Read a property on D-Bus
 *
 * It will throw on errors
 *
 * @tparam T - The type of the property
 * @param[in] bus - The sdbusplus object
 * @param[in] service - The service
 * @param[in] path - The path
 * @param[in] interface - The interface
 * @param[in] property - The property
 *
 * @return T - The property  value
 */
template <typename T>
T getProperty(sdbusplus::bus_t& bus, const std::string& service,
              const std::string& path, const std::string& interface,
              const std::string& property)
{
    auto method = bus.new_method_call(service.c_str(), path.c_str(),
                                      "org.freedesktop.DBus.Properties", "Get");

    method.append(interface, property);
    auto reply = bus.call(method);

    auto value = reply.unpack<std::variant<T>>();

    return std::get<T>(value);
}

/**
 * @brief Looks up the D-Bus service name for the path and interface
 *
 * It will throw if the service doesn't exist.
 *
 * @param[in] bus - The sdbusplus object
 * @param[in] path - The path
 * @param[in] interface - The interface
 *
 * @return The service name
 */
std::string getService(sdbusplus::bus_t& bus, const std::string& path,
                       const std::string& interface);

namespace additional_data
{
/** @brief Pull out metadata name and value from the string
 *         <metadata name>=<metadata value>
 *  @param [in] data - metadata key=value entries
 *  @return map of metadata name:value
 */
auto parse(const std::vector<std::string>& data)
    -> std::map<std::string, std::string>;
/** @brief Combine the metadata keys and values from the map
 *         into a vector of strings that look like:
 *            "<metadata name>=<metadata value>"
 *  @param [in] data - metadata key:value map
 *  @return vector of "key=value" strings
 */
auto combine(const std::map<std::string, std::string>& data)
    -> std::vector<std::string>;
} // namespace additional_data

} // namespace phosphor::logging::util
