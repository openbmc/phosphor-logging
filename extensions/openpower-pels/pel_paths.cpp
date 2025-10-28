// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "config.h"

#include "paths.hpp"

#include <filesystem>

namespace openpower
{
namespace pels
{

namespace fs = std::filesystem;
static constexpr size_t defaultRepoSize = 20 * 1024 * 1024;
static constexpr size_t defaultMaxNumPELs = 3000;

fs::path getPELIDFile()
{
    fs::path logIDPath{phosphor::logging::paths::extension()};
    logIDPath /= fs::path{"pels"} / fs::path{"pelID"};
    return logIDPath;
}

fs::path getPELRepoPath()
{
    std::filesystem::path repoPath{phosphor::logging::paths::extension()};
    repoPath /= "pels";
    return repoPath;
}

fs::path getPELReadOnlyDataPath()
{
    return std::filesystem::path{"/usr/share/phosphor-logging/pels"};
}

size_t getPELRepoSize()
{
    // For now, always use 20MB, revisit in the future if different
    // systems need different values so that we only put PEL
    // content into configure.ac when absolutely necessary.
    return defaultRepoSize;
}

size_t getMaxNumPELs()
{
    // Hardcode using the same reasoning as the repo size field.
    return defaultMaxNumPELs;
}

} // namespace pels
} // namespace openpower
