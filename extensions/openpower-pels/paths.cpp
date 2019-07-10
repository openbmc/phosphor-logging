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

} // namespace pels

} // namespace openpower
