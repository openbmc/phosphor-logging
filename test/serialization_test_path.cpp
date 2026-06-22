#include "elog_entry.hpp"
#include "elog_serialize.hpp"
#include "serialization_tests.hpp"

namespace phosphor
{
namespace logging
{
namespace test
{

TEST_F(TestSerialization, testPath)
{
    auto id = 99;
    // Use fully initialized constructor because serialize() reads all
    // persisted fields, not just ID.
    phosphor::logging::AssociationList associations{};
    std::map<std::string, std::string> testData{};
    uint64_t timestamp{0};
    std::string message{"path test"};
    std::string fwLevel{"test-fw"};
    std::string objectPath =
        std::string(OBJ_ENTRY) + '/' + std::to_string(id);
    std::string inputPath = getEntrySerializePath(id, TestSerialization::dir);
    auto e = std::make_unique<Entry>(
        bus, objectPath, id, timestamp, Entry::Level::Informational,
        std::move(message), std::move(testData), std::move(associations),
        fwLevel, inputPath, manager);
    auto path = serialize(*e, TestSerialization::dir);
    EXPECT_EQ(path.c_str(), TestSerialization::dir / std::to_string(id));
}

} // namespace test
} // namespace logging
} // namespace phosphor
