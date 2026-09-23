#include "oem_registry.hpp"

#include <phosphor-logging/lg2.hpp>

#include <algorithm>
#include <cctype>
#include <system_error>

namespace phosphor::logging::cper
{

PHOSPHOR_LOG2_USING;

namespace
{

OemRegistry* registry = nullptr;
std::string sourceLib{};

auto libname(const std::filesystem::path& path) -> std::string
{
    return path.filename().string();
}

/** Normalize a GUID for registry lookup (lowercase). */
auto normalize(std::string_view guid) -> std::string
{
    std::string key{guid};
    std::ranges::transform(key, key.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return key;
}

} // namespace

OemRegistry::OemRegistry()
{
    registry = this;
}

OemRegistry::~OemRegistry()
{
    if (registry == this)
    {
        registry = nullptr;
    }
}

auto OemRegistry::current() noexcept -> OemRegistry*
{
    return registry;
}

void OemRegistry::add(std::string guid, v1::Handler handler)
{
    auto key = normalize(guid);
    if (key.empty())
    {
        warning("Ignoring OEM handler with empty GUID: {LIB}", "LIB",
                sourceLib);
        return;
    }
    auto [it, inserted] = handlers.emplace(
        key, std::make_unique<const Match>(
                 Match{.handler = std::move(handler), .source = sourceLib}));
    if (!inserted)
    {
        warning("Ignoring duplicate OEM handler for GUID {GUID}: {LIB}", "GUID",
                key, "LIB", sourceLib);
    }
}

auto OemRegistry::find(const std::string& guid) const -> const Match*
{
    auto it = handlers.find(normalize(guid));
    if (it == handlers.end())
    {
        return nullptr;
    }
    return it->second.get();
}

void OemRegistry::load(const std::filesystem::path& dir)
{
    std::error_code ec{};
    if (!std::filesystem::is_directory(dir, ec))
    {
        warning("OEM plugin directory {DIR} not found, skipping", "DIR",
                dir.string());
        return;
    }

    std::vector<std::filesystem::path> libs{};
    for (const auto& entry : std::filesystem::directory_iterator(dir, ec))
    {
        if (ec)
        {
            warning("Could not scan OEM plugin directory {DIR}: {ERR}", "DIR",
                    dir.string(), "ERR", ec.message());
            return;
        }
        if (entry.path().extension() == ".so")
        {
            libs.push_back(entry.path());
        }
    }
    std::ranges::sort(libs);

    for (const auto& path : libs)
    {
        sourceLib = libname(path);
        auto* handle = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
        sourceLib.clear();
        if (handle == nullptr)
        {
            warning("Could not load OEM plugin {LIB}: {ERR}", "LIB",
                    libname(path), "ERR", dlerror());
            continue;
        }
        this->libs.emplace_back(handle);
        info("Loaded OEM plugin {LIB}", "LIB", libname(path));
    }
}

} // namespace phosphor::logging::cper

namespace phosphor::logging::cper::v1
{

void add(Guid guid, Handler handler)
{
    auto* registry = OemRegistry::current();
    if (registry == nullptr)
    {
        throw std::logic_error("v1::add called with no OemRegistry");
    }
    registry->add(std::move(guid), std::move(handler));
}

void add(std::vector<Guid> guids, Handler handler)
{
    auto* registry = OemRegistry::current();
    if (registry == nullptr)
    {
        throw std::logic_error("v1::add called with no OemRegistry");
    }
    for (auto& guid : guids)
    {
        registry->add(std::move(guid), handler);
    }
}

} // namespace phosphor::logging::cper::v1
