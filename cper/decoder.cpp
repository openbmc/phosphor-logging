#include "decoder.hpp"

#include <libcper.hpp>
#include <nlohmann/json.hpp>
#include <phosphor-logging/commit.hpp>
#include <phosphor-logging/lg2.hpp>
#include <xyz/openbmc_project/Logging/Extension/CPER/Raw/common.hpp>
#include <xyz/openbmc_project/State/CPER/event.hpp>

#include <utility>

namespace phosphor::logging::cper
{

namespace dbus::cper
{
using Raw =
    sdbusplus::common::xyz::openbmc_project::logging::extension::cper::Raw;
} // namespace dbus::cper

namespace
{

template <typename Event, typename... Extensions>
auto extend(Event&& event, Extensions&&... extensions) -> Event&&
{
    (event.extend(extensions), ...);
    return std::forward<Event>(event);
}

template <typename... Extensions>
auto buildEvent(Decoder::Severity severity, sdbusplus::object_path&& source,
                Extensions&&... extensions)
    -> std::unique_ptr<sdbusplus::exception::generated_event_base>;

template <typename Event, typename... Extensions>
auto buildEventFor(sdbusplus::object_path&& source, Extensions&&... extensions)
    -> Event
{
    return extend(Event{"SOURCE", std::move(source)},
                  std::forward<Extensions>(extensions)...);
}

} // namespace

PHOSPHOR_LOG2_USING;

Decoder::~Decoder()
{
    stop();
}

void Decoder::start()
{
    if (started)
    {
        return;
    }
    started = true;
    thread = std::thread{[this] { run(); }};
}

void Decoder::stop()
{
    if (!started)
    {
        return;
    }
    worker.spawn(
        sdbusplus::async::execution::just() |
        sdbusplus::async::execution::then([this]() { worker.request_stop(); }));
    if (thread.joinable())
    {
        thread.join();
    }
    started = false;
}

void Decoder::queue(sdbusplus::object_path&& source, ContentType type,
                    std::vector<std::uint8_t>&& raw)
{
    worker.spawn(process(std::move(source), type, std::move(raw)));
}

auto Decoder::parseLibCPER(ContentType type, std::span<const std::uint8_t> raw)
    -> std::tuple<OemData, Guid, Severity>
{
    auto [json, guidPath, severityPath] =
        [&]() -> std::tuple<nlohmann::json, std::string, std::string> {
        switch (type)
        {
            case ContentType::CPER:
                return {libcper::ir::parseCPER(raw),
                        "/header/notificationType/guid",
                        "/header/severity/code"};
            case ContentType::CPERSection:
                return {libcper::ir::parseCPERSection(raw),
                        "/sectionDescriptor/sectionType/data",
                        "/sectionDescriptor/severity/code"};
        }
        return {nullptr, "", ""};
    }();

    if (!json.is_object())
    {
        return {OemData{}, Guid{}, Severity::Recoverable};
    }

    json["@odata.type"] = "#OpenBMC.v0_0_0.CPER";
    Guid guid =
        json.value(nlohmann::json::json_pointer(guidPath), std::string{});

    return {OemData{{"OpenBMC", json.dump()}}, std::move(guid),
            severity(json.value(nlohmann::json::json_pointer(severityPath),
                                uint32_t{}))};
}

auto Decoder::severity(uint32_t code) -> Severity
{
    if (code > static_cast<uint32_t>(Severity::Informational))
    {
        return Severity::Recoverable;
    }
    return static_cast<Severity>(code);
}

auto Decoder::processedProps(ContentType type, OemData oem, Guid guid)
    -> ProcessedProps
{
    ProcessedProps props{};
    props.diagnostic_data_type = type;
    props.oem = std::move(oem);
    switch (type)
    {
        case ContentType::CPER:
            props.notification_type = std::move(guid);
            break;
        case ContentType::CPERSection:
            props.section_type = std::move(guid);
            break;
    }
    return props;
}

auto Decoder::process(sdbusplus::object_path source, ContentType type,
                      std::vector<std::uint8_t> raw) -> sdbusplus::async::task<>
{
    auto result = parseLibCPER(type, raw);
    // GCC < 16.2 has a bug where the structured binding above doesn't
    // work directly and GCC incorrectly sees the result as un-initilized.
    //     auto [oem, guid, severity] = parseLibCPER(...);
    // Workaround by manually unpacking the values here.
    auto oem = std::move(std::get<0>(result));
    auto guid = std::move(std::get<1>(result));
    auto severity = std::move(std::get<2>(result));

    ProcessedProps processed =
        processedProps(type, std::move(oem), std::move(guid));

    co_await committer(
        worker, std::move(*buildEvent(
                    severity, std::move(source), std::move(processed),
                    dbus::cper::Raw::properties_t{.data = std::move(raw)})));
}

void Decoder::run()
{
    worker.run();
}

void Decoder::setCommitter(Committer handler)
{
    committer = std::move(handler);
}

auto Decoder::defaultCommitter(
    sdbusplus::async::context& ctx,
    sdbusplus::exception::generated_event_base&& event)
    -> sdbusplus::async::task<void>
{
    co_await lg2::commit(ctx, std::move(event));
}

namespace
{

template <typename... Extensions>
auto buildEvent(Decoder::Severity severity, sdbusplus::object_path&& source,
                Extensions&&... extensions)
    -> std::unique_ptr<sdbusplus::exception::generated_event_base>
{
    using namespace sdbusplus::error::xyz::openbmc_project::state::CPER;
    switch (severity)
    {
        case Decoder::Severity::Fatal:
            return std::make_unique<Fatal>(buildEventFor<Fatal>(
                std::move(source), std::forward<Extensions>(extensions)...));
        case Decoder::Severity::Recoverable:
            return std::make_unique<Recoverable>(buildEventFor<Recoverable>(
                std::move(source), std::forward<Extensions>(extensions)...));
        case Decoder::Severity::Corrected:
            return std::make_unique<Corrected>(buildEventFor<Corrected>(
                std::move(source), std::forward<Extensions>(extensions)...));
        case Decoder::Severity::Informational:
            return std::make_unique<Informational>(buildEventFor<Informational>(
                std::move(source), std::forward<Extensions>(extensions)...));
    }
    return std::make_unique<Recoverable>(buildEventFor<Recoverable>(
        std::move(source), std::forward<Extensions>(extensions)...));
}

} // namespace

} // namespace phosphor::logging::cper
