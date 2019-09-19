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
