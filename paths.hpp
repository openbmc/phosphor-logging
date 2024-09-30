#pragma once

#include <filesystem>
#include <string>

extern const char* PERSIST_PATH_ROOT;

namespace phosphor::logging::paths
{

auto error() -> std::filesystem::path;
auto extension() -> std::filesystem::path;

} // namespace phosphor::logging::paths
