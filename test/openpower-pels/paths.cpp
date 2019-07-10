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

} // namespace pels
} // namespace openpower
