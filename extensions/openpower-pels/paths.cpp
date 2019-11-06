/**
 * Copyright © 2019 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "config.h"

#include "paths.hpp"

#include <filesystem>

namespace openpower
{
namespace pels
{

namespace fs = std::filesystem;

fs::path getPELIDFile()
{
    fs::path logIDPath{EXTENSION_PERSIST_DIR};
    logIDPath /= fs::path{"pels"} / fs::path{"pelID"};
    return logIDPath;
}

fs::path getPELRepoPath()
{
    std::filesystem::path repoPath{EXTENSION_PERSIST_DIR};
    repoPath /= "pels";
    return repoPath;
}

fs::path getMessageRegistryPath()
{
    return std::filesystem::path{"/usr/share/phosphor-logging/pels"};
}

} // namespace pels
} // namespace openpower
