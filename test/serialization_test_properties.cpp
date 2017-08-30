#include "serialization_tests.hpp"
#include "elog_entry.hpp"
#include "elog_serialize.hpp"

namespace phosphor
{
namespace logging
{
namespace test
{

TEST_F(TestSerialization, testProperties)
{
    using  Entry = internel::Entry;
    auto id = 99;
    phosphor::logging::AssociationList assocations{};
    std::vector<std::string> testData{"additional", "data"};
    uint64_t timestamp{100};
    std::string message{"test error"};
    auto input = std::make_unique<Entry>(
                     bus,
                     std::string(OBJ_ENTRY) + '/' + std::to_string(id),
                     id,
                     timestamp,
                     Entry::Level::Informational,
                     std::move(message),
                     std::move(testData),
                     std::move(assocations),
                     manager);
    auto path = serialize(*input, TestSerialization::dir);

    auto idStr = path.filename().c_str();
    id = std::stol(idStr);
    auto output = std::make_unique<Entry>(
                      bus,
                      std::string(OBJ_ENTRY) + '/' + idStr,
                      id,
                      manager);
    deserialize(path, *output);

    EXPECT_EQ(input->id(), output->id());
    EXPECT_EQ(input->severity(), output->severity());
    EXPECT_EQ(input->timestamp(), output->timestamp());
    EXPECT_EQ(input->message(), output->message());
    EXPECT_EQ(input->additionalData(), output->additionalData());
    EXPECT_EQ(input->resolved(), output->resolved());
    EXPECT_EQ(input->associations(), output->associations());
}

} // namespace test
} // namespace logging
} // namespace phosphor


