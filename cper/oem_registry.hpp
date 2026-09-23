#pragma once

#include <dlfcn.h>

#include <phosphor-logging/cper.hpp>

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace phosphor::logging::cper
{

/** Owns OEM decoder plugins loaded from shared libraries.
 *
 * Each plugin registers `v1::Handler`s for section-type GUIDs via static
 * `v1::Register` instances that fire during `dlopen`. The registry maps
 * normalized GUIDs to handlers; on a GUID collision the first registration
 * wins and later ones are ignored with a warning.
 */
class OemRegistry
{
  public:
    OemRegistry();
    OemRegistry(const OemRegistry&) = delete;
    OemRegistry& operator=(const OemRegistry&) = delete;
    OemRegistry(OemRegistry&&) = delete;
    OemRegistry& operator=(OemRegistry&&) = delete;
    ~OemRegistry();

    /** Default directory scanned for OEM decoder plugins. */
    static constexpr std::string_view defaultDir =
        "/usr/lib/phosphor-logging/cper-decoders";

    /** A handler plus the library it was registered from. */
    struct Match
    {
        v1::Handler handler;
        std::string source;
    };

    /** Register a handler for one section-type GUID. */
    void add(std::string guid, v1::Handler handler);

    /** Look up the handler for a section-type GUID, if any. */
    auto find(const std::string& guid) const -> const Match*;

    /** `dlopen` every `*.so` in `dir`, sorted by filename. */
    void load(const std::filesystem::path& dir);

    /** The registry accepting `v1::add` calls, if any. */
    static auto current() noexcept -> OemRegistry*;

  private:
    struct LibCloser
    {
        void operator()(void* handle) const noexcept
        {
            if (handle != nullptr)
            {
                dlclose(handle);
            }
        }
    };
    // Declared before handlers so handlers (which may reference plugin code)
    // are destroyed before the libraries are closed.
    std::vector<std::unique_ptr<void, LibCloser>> libs{};
    std::unordered_map<std::string, std::unique_ptr<const Match>> handlers{};
};

} // namespace phosphor::logging::cper
