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

// Commit the matching State.CPER event with extensions.
template <typename... Extensions>
auto commit(sdbusplus::async::context& ctx, Decoder::Severity severity,
            sdbusplus::object_path&& source, Extensions&&... extensions)
    -> sdbusplus::async::task<void>;

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
    worker.request_stop();
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
    auto [oem, guid, severity] = parseLibCPER(type, raw);
    ProcessedProps processed =
        processedProps(type, std::move(oem), std::move(guid));

    co_await commit(worker, severity, std::move(source), std::move(processed),
                    dbus::cper::Raw::properties_t{.data = std::move(raw)});
}

void Decoder::run()
{
    worker.run();
}

namespace
{

template <typename Event, typename... Extensions>
auto commit(sdbusplus::async::context& ctx, sdbusplus::object_path&& source,
            Extensions&&... extensions) -> sdbusplus::async::task<void>
{
    Event event{"SOURCE", std::move(source)};
    (event.extend(extensions), ...);
    co_await lg2::commit(ctx, std::move(event));
}

template <typename... Extensions>
auto commit(sdbusplus::async::context& ctx, Decoder::Severity severity,
            sdbusplus::object_path&& source, Extensions&&... extensions)
    -> sdbusplus::async::task<void>
{
    using namespace sdbusplus::error::xyz::openbmc_project::state::CPER;
    switch (severity)
    {
        case Decoder::Severity::Fatal:
            co_await commit<Fatal>(ctx, std::move(source),
                                   std::forward<Extensions>(extensions)...);
            break;
        case Decoder::Severity::Recoverable:
            co_await commit<Recoverable>(
                ctx, std::move(source),
                std::forward<Extensions>(extensions)...);
            break;
        case Decoder::Severity::Corrected:
            co_await commit<Corrected>(ctx, std::move(source),
                                       std::forward<Extensions>(extensions)...);
            break;
        case Decoder::Severity::Informational:
            co_await commit<Informational>(
                ctx, std::move(source),
                std::forward<Extensions>(extensions)...);
            break;
    }
}

} // namespace

} // namespace phosphor::logging::cper
