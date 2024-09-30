#include "config.h"

#include "paths.hpp"

namespace phosphor::logging::paths
{
auto error() -> std::filesystem::path
{
    return std::filesystem::path(PERSIST_PATH_ROOT) / "errors";
}
auto extension() -> std::filesystem::path
{
    return std::filesystem::path(PERSIST_PATH_ROOT) / "extensions";
}
} // namespace phosphor::logging::paths
