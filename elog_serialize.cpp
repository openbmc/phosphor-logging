#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/archives/binary.hpp>
#include <fstream>
#include "elog_serialize.hpp"

namespace phosphor
{
namespace logging
{

fs::path serialize(const Entry& e, const fs::path& dir)
{
    auto path = dir / std::to_string(e.id());
    std::ofstream os(path.c_str(), std::ios::binary);
    cereal::BinaryOutputArchive oarchive(os);
    oarchive(e);
    return path;
}

bool deserialize(const fs::path& path, Entry& e)
{
    if (fs::exists(path))
    {
        std::ifstream is(path.c_str(), std::ios::in | std::ios::binary);
        cereal::BinaryInputArchive iarchive(is);
        iarchive(e);
        return true;
    }
    return false;
}

} // namespace logging
} // namespace phosphor
