// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "extensions/openpower-pels/paths.hpp"

#include <filesystem>

namespace openpower
{
namespace pels
{

// Use paths that work in unit tests.

std::filesystem::path getPELIDFile()
{
    static std::string idFile;

    if (idFile.empty())
    {
        char templ[] = "/tmp/logidtestXXXXXX";
        std::filesystem::path dir = mkdtemp(templ);
        idFile = dir / "logid";
    }
    return idFile;
}

std::filesystem::path getPELRepoPath()
{
    static std::string repoPath;

    if (repoPath.empty())
    {
        char templ[] = "/tmp/repopathtestXXXXXX";
        std::filesystem::path dir = mkdtemp(templ);
        repoPath = dir;
    }
    return repoPath;
}

std::filesystem::path getPELReadOnlyDataPath()
{
    static std::string dataPath;

    if (dataPath.empty())
    {
        char templ[] = "/tmp/pelrodatatestXXXXXX";
        dataPath = mkdtemp(templ);
    }

    return dataPath;
}

size_t getPELRepoSize()
{
    // 100KB
    return 100 * 1024;
}

size_t getMaxNumPELs()
{
    return 100;
}

} // namespace pels
} // namespace openpower
