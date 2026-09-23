#include "decoder.hpp"

#include <libcper.hpp>
#include <nlohmann/json.hpp>
#include <phosphor-logging/commit.hpp>
#include <phosphor-logging/lg2.hpp>
#include <xyz/openbmc_project/Logging/Extension/CPER/Raw/common.hpp>
#include <xyz/openbmc_project/State/CPER/event.hpp>

#include <algorithm>
#include <unordered_map>
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

void Decoder::start(const std::filesystem::path& pluginDir)
{
    if (started)
    {
        return;
    }
    oemRegistry.load(pluginDir);
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
    -> LibCperResult
{
    auto [json, guidPath, severityPath, sectionPath] = [&]()
        -> std::tuple<nlohmann::json, std::string, std::string, std::string> {
        switch (type)
        {
            case ContentType::CPER:
                return {libcper::ir::parseCPER(raw),
                        "/header/notificationType/guid",
                        "/header/severity/code", "/sectionDescriptors"};
            case ContentType::CPERSection:
                return {libcper::ir::parseCPERSection(raw),
                        "/sectionDescriptor/sectionType/data",
                        "/sectionDescriptor/severity/code",
                        "/sectionDescriptor"};
        }
        return {nullptr, "", "", ""};
    }();

    if (!json.is_object())
    {
        return {};
    }

    json["@odata.type"] = "#OpenBMC.v0_0_0.CPER";
    LibCperResult result{};
    result.guid =
        json.value(nlohmann::json::json_pointer(guidPath), std::string{});
    result.severity = severity(
        json.value(nlohmann::json::json_pointer(severityPath), uint32_t{}));
    result.oem = OemData{{"OpenBMC", json.dump()}};

    // Collect every section-type GUID so OEM plugins can be dispatched.
    const auto* descriptors = [&]() -> const nlohmann::json* {
        auto ptr = nlohmann::json::json_pointer(sectionPath);
        if (!json.contains(ptr))
        {
            return nullptr;
        }
        return &json.at(ptr);
    }();
    if (descriptors != nullptr)
    {
        const auto pushGuid = [&result](const nlohmann::json& descriptor) {
            if (!descriptor.is_object())
            {
                return;
            }
            auto guid = descriptor.value(
                nlohmann::json::json_pointer("/sectionType/data"),
                std::string{});
            if (!guid.empty())
            {
                result.sectionGuids.push_back(std::move(guid));
            }
        };
        if (descriptors->is_array())
        {
            for (const auto& descriptor : *descriptors)
            {
                pushGuid(descriptor);
            }
        }
        else
        {
            pushGuid(*descriptors);
        }
    }

    return result;
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
    runOem(type, raw, result);
    ProcessedProps processed =
        processedProps(type, std::move(result.oem), std::move(result.guid));

    co_await committer(
        worker, std::move(*buildEvent(
                    result.severity, std::move(source), std::move(processed),
                    dbus::cper::Raw::properties_t{.data = std::move(raw)})));
}

void Decoder::runOem(ContentType type, std::span<const std::uint8_t> raw,
                     LibCperResult& result)
{
    static_assert(std::is_same_v<OemData, v1::Oem>);
    static_assert(std::is_same_v<Guid, v1::Guid>);
    static_assert(std::is_same_v<ContentType, v1::Type>);

    // Dedupe handlers: one plugin registered for several GUIDs in this
    // record still gets exactly one call, with its matched subset.
    std::unordered_map<const OemRegistry::Match*, std::vector<v1::Guid>>
        calls{};
    for (const auto& guid : result.sectionGuids)
    {
        auto* match = oemRegistry.find(guid);
        if (match == nullptr)
        {
            continue;
        }
        calls[match].push_back(guid);
    }
    if (calls.empty())
    {
        return;
    }

    // Deterministic order for reproducible first-wins merges.
    std::vector<std::pair<const OemRegistry::Match*, std::vector<v1::Guid>>>
        ordered(std::from_range, calls);
    std::ranges::sort(ordered, [](const auto& a, const auto& b) {
        return a.first->source < b.first->source;
    });

    for (const auto& [match, guids] : ordered)
    {
        v1::Entry entry{};
        try
        {
            entry = match->handler(static_cast<v1::Type>(type), raw, result.oem,
                                   guids);
        }
        catch (const std::exception& e)
        {
            warning("OEM plugin {LIB} failed: {ERR}", "LIB", match->source,
                    "ERR", e.what());
            continue;
        }
        catch (...)
        {
            warning("OEM plugin {LIB} failed with unknown error", "LIB",
                    match->source);
            continue;
        }
        if (entry.key.empty())
        {
            warning("OEM plugin {LIB} returned an empty key, skipping", "LIB",
                    match->source);
            continue;
        }
        if (entry.key == "OpenBMC")
        {
            warning("OEM plugin {LIB} used reserved key OpenBMC, skipping",
                    "LIB", match->source);
            continue;
        }
        if (result.oem.contains(entry.key))
        {
            warning("Duplicate OEM key {KEY} from {LIB}, keeping first", "KEY",
                    entry.key, "LIB", match->source);
            continue;
        }
        try
        {
            result.oem.emplace(entry.key, entry.value.dump());
        }
        catch (const std::exception& e)
        {
            warning("OEM plugin {LIB} entry {KEY} failed to dump: {ERR}", "LIB",
                    match->source, "KEY", entry.key, "ERR", e.what());
        }
    }
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
