#include "config.h"

#include "paths.hpp"

namespace phosphor::logging::paths
{
auto error() -> std::filesystem::path
{
    return std::filesystem::path(PERSIST_PATH_ROOT) / "errors";
}

auto error_json() -> std::filesystem::path
{
    return std::filesystem::path(PERSIST_PATH_ROOT) / "errors-json";
}

auto extension() -> std::filesystem::path
{
    return std::filesystem::path(PERSIST_PATH_ROOT) / "extensions";
}

auto artifact() -> std::filesystem::path
{
    return std::filesystem::path(PERSIST_PATH_ROOT) / "artifacts";
}

} // namespace phosphor::logging::paths
