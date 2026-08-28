#include "log_create_extensions.hpp"

#include <nlohmann/json.hpp>
#include <xyz/openbmc_project/Logging/Extension/CPER/Processed/common.hpp>
#include <xyz/openbmc_project/Logging/Extension/CPER/Raw/common.hpp>

#include <functional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

namespace
{

using HandlerMap = std::unordered_map<
    std::string, std::function<void(sdbusplus::exception::generated_event_base&,
                                    const nlohmann::json&)>>;

template <typename T>
auto extender() -> HandlerMap::value_type
{
    return {
        std::string(T::interface),
        [](auto& event, const auto& json) {
            typename T::properties_t properties;
            json.get_to(properties);
            event.extend(properties);
        },
    };
}

namespace extension =
    sdbusplus::common::xyz::openbmc_project::logging::extension;

const HandlerMap handlers = {
    extender<extension::cper::Processed>(),
    extender<extension::cper::Raw>(),
};

} // namespace

void extendLogEvent(sdbusplus::exception::generated_event_base& event,
                    const std::string& interface, const nlohmann::json& json)
{
    if (auto handler = handlers.find(interface); handler != handlers.end())
    {
        handler->second(event, json);
        return;
    }

    throw std::invalid_argument("Unknown extension interface: " + interface);
}
