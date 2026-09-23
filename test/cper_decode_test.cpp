#include "../cper/decoder.hpp"

#include <nlohmann/json.hpp>
#include <phosphor-logging/cper.hpp>

#include <array>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <future>
#include <memory>
#include <span>
#include <stdexcept>
#include <vector>

#include <gtest/gtest.h>

namespace
{

// Binary decode of libcper examples/memory.cperhex.
constexpr std::array<std::uint8_t, 280> memoryCper{
    0x43, 0x50, 0x45, 0x52, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00,
    0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x18, 0x01, 0x00, 0x00,
    0x19, 0x00, 0x01, 0x00, 0x17, 0x01, 0x32, 0x99, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xfb, 0x06, 0x5a, 0x72, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc8, 0x00, 0x00, 0x00,
    0x50, 0x00, 0x00, 0x00, 0xa9, 0x72, 0x03, 0x00, 0x0b, 0x00, 0x00, 0x00,
    0x14, 0x11, 0xbc, 0xa5, 0x64, 0x6f, 0xde, 0x4e, 0xb8, 0x63, 0x3e, 0x83,
    0xed, 0x7c, 0x83, 0xb1, 0x7d, 0x6e, 0x47, 0x4c, 0xb9, 0x44, 0xab, 0x3e,
    0x6f, 0x24, 0x14, 0x38, 0x84, 0x8e, 0xd4, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x2b, 0x71, 0x24, 0x60, 0x34, 0x70, 0x47, 0x78, 0x27, 0x53, 0x36, 0x40,
    0x77, 0x59, 0x7c, 0x35, 0x67, 0x70, 0x21, 0x00, 0x55, 0x55, 0x27, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x6b, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x2a, 0x7e, 0xd9, 0x50, 0x09, 0x1e, 0x2d, 0x63, 0xa6, 0x8e, 0x25, 0x94,
    0xf5, 0xe0, 0x41, 0x97, 0x75, 0x98, 0xe5, 0xd9, 0x31, 0x47, 0x80, 0xcd,
    0x54, 0xfb, 0x6e, 0x61, 0x2b, 0x6b, 0xbe, 0x05, 0x8d, 0x10, 0xe4, 0x39,
    0x55, 0xaf, 0x54, 0xba, 0x86, 0x94, 0xbc, 0xde, 0x15, 0x31, 0xb8, 0x44,
    0x82, 0xc0, 0x60, 0x6f, 0xba, 0xb4, 0x9e, 0xb5, 0x00, 0xc0, 0xce, 0x56,
    0x8d, 0x13, 0x7c, 0x52};

} // namespace

namespace phosphor::logging::cper::test
{

namespace v1 = phosphor::logging::cper::v1;

class DecoderTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        decoder = std::make_unique<Decoder>();
        decoder->start();
    }

    void TearDown() override
    {
        decoder->stop();
        decoder.reset();
    }

    auto parseLibCPER(ContentType type, std::span<const std::uint8_t> raw)
        -> Decoder::LibCperResult
    {
        return decoder->parseLibCPER(type, raw);
    }

    static auto severity(uint32_t code) -> Decoder::Severity
    {
        return Decoder::severity(code);
    }

    auto processedProps(ContentType type, Decoder::OemData data,
                        Decoder::Guid guid) -> Decoder::ProcessedProps
    {
        return decoder->processedProps(type, std::move(data), std::move(guid));
    }

    void setCommitter(Decoder::Committer handler)
    {
        decoder->setCommitter(std::move(handler));
    }

    void runOem(ContentType type, std::span<const std::uint8_t> raw,
                Decoder::LibCperResult& result)
    {
        decoder->runOem(type, raw, result);
    }

    void addOem(std::string guid, v1::Handler handler)
    {
        decoder->oemRegistry.add(std::move(guid), std::move(handler));
    }

    // Copy the test plugin into a temp dir and load it for real via dlopen.
    auto loadTestPlugin() -> std::filesystem::path
    {
        auto dir =
            std::filesystem::temp_directory_path() /
            ("cper-oem-test-" +
             std::to_string(::testing::UnitTest::GetInstance()->random_seed()));
        std::filesystem::create_directories(dir);
        // shared_module names the output libcper_oem_test_plugin.so.
        auto dest = dir / "libcper_oem_test_plugin.so";
        std::filesystem::copy_file(CPER_OEM_TEST_PLUGIN, dest);
        decoder->oemRegistry.load(dir);
        return dir;
    }

    // Queue on the worker and wait for the captured event (30s timeout).
    auto runProcess(sdbusplus::object_path source, ContentType type,
                    std::vector<std::uint8_t> raw)
    {
        std::promise<nlohmann::json> done{};
        auto result = done.get_future();
        setCommitter(
            [&done](auto&, auto&& event) -> sdbusplus::async::task<void> {
                done.set_value(event.to_json());
                co_return;
            });
        decoder->queue(std::move(source), type, std::move(raw));
        if (result.wait_for(std::chrono::seconds(30)) !=
            std::future_status::ready)
        {
            throw std::runtime_error("Timed out waiting for CPER decode");
        }
        return result.get();
    }

    std::unique_ptr<Decoder> decoder{};
};

TEST_F(DecoderTest, MemoryRecordParses)
{
    auto result = parseLibCPER(ContentType::CPER, memoryCper);

    // Null notification-type GUID, Corrected severity.
    EXPECT_EQ(result.guid, "00000000-0000-0000-0000-000000000000");
    EXPECT_EQ(result.severity, Decoder::Severity::Corrected);

    ASSERT_EQ(result.oem.size(), 1);
    ASSERT_TRUE(result.oem.contains("OpenBMC"));
    const auto inner = nlohmann::json::parse(result.oem.at("OpenBMC"));
    ASSERT_TRUE(inner.is_object());
    EXPECT_EQ(inner.at("@odata.type"), "#OpenBMC.v0_0_0.CPER");
    EXPECT_EQ(inner.at("header").at("recordLength"), 280);
    EXPECT_EQ(inner.at("header").at("sectionCount"), 1);
    EXPECT_EQ(inner.at("sections")
                  .at(0)
                  .at("Memory")
                  .at("memoryErrorType")
                  .at("name"),
              "Unknown");

    // One section GUID extracted for OEM dispatch.
    ASSERT_EQ(result.sectionGuids.size(), 1);
    EXPECT_EQ(result.sectionGuids.front(),
              "a5bc1114-6f64-4ede-b863-3e83ed7c83b1");
}

TEST_F(DecoderTest, GarbageIsEmpty)
{
    const std::vector<std::uint8_t> garbage{'n', 'o', 't', ' ',
                                            'c', 'p', 'e', 'r'};
    auto result = parseLibCPER(ContentType::CPER, garbage);
    EXPECT_TRUE(result.oem.empty());
    EXPECT_TRUE(result.guid.empty());
    EXPECT_TRUE(result.sectionGuids.empty());
    EXPECT_EQ(result.severity, Decoder::Severity::Recoverable);
}

TEST_F(DecoderTest, ProcessCommitsCorrectedEvent)
{
    auto json = runProcess(
        sdbusplus::object_path{"/xyz/openbmc_project/inventory/DIMM_0"},
        ContentType::CPER,
        std::vector<std::uint8_t>(std::from_range, memoryCper));

    // Corrected severity.
    ASSERT_TRUE(json.contains("xyz.openbmc_project.State.CPER.Corrected"));
    const auto& self = json.at("xyz.openbmc_project.State.CPER.Corrected");
    EXPECT_EQ(self.at("SOURCE"), "/xyz/openbmc_project/inventory/DIMM_0");

    // Both extensions under _EXTENSIONS.
    ASSERT_TRUE(self.contains("_EXTENSIONS"));
    const auto& extensions = self.at("_EXTENSIONS");
    ASSERT_TRUE(extensions.contains(
        "xyz.openbmc_project.Logging.Extension.CPER.Processed"));
    const auto& processed =
        extensions.at("xyz.openbmc_project.Logging.Extension.CPER.Processed");
    EXPECT_EQ(processed.at("DiagnosticDataType"),
              "xyz.openbmc_project.Logging.CPER.Types.ContentType.CPER");
    EXPECT_EQ(processed.at("NotificationType"),
              "00000000-0000-0000-0000-000000000000");
    ASSERT_TRUE(processed.contains("Oem"));
    ASSERT_TRUE(processed.at("Oem").contains("OpenBMC"));

    ASSERT_TRUE(
        extensions.contains("xyz.openbmc_project.Logging.Extension.CPER.Raw"));
    const auto& raw =
        extensions.at("xyz.openbmc_project.Logging.Extension.CPER.Raw");
    ASSERT_TRUE(raw.contains("Data"));
    EXPECT_EQ(raw.at("Data").size(), memoryCper.size());
}

TEST_F(DecoderTest, SeverityMapping)
{
    using enum Decoder::Severity;
    EXPECT_EQ(severity(0), Recoverable);
    EXPECT_EQ(severity(1), Fatal);
    EXPECT_EQ(severity(2), Corrected);
    EXPECT_EQ(severity(3), Informational);
    // Out-of-range falls back to Recoverable.
    EXPECT_EQ(severity(4), Recoverable);
    EXPECT_EQ(severity(42), Recoverable);
}

TEST_F(DecoderTest, PropertiesCarriesOem)
{
    auto result = parseLibCPER(ContentType::CPER, memoryCper);
    EXPECT_EQ(result.severity, Decoder::Severity::Corrected);
    auto props = processedProps(ContentType::CPER, std::move(result.oem),
                                std::move(result.guid));
    EXPECT_EQ(props.diagnostic_data_type, ContentType::CPER);
    EXPECT_EQ(props.notification_type, "00000000-0000-0000-0000-000000000000");
    EXPECT_TRUE(props.section_type.empty());
    ASSERT_EQ(props.oem.size(), 1);
    EXPECT_TRUE(props.oem.contains("OpenBMC"));
}

TEST_F(DecoderTest, OemHandlerMergesEntry)
{
    auto result = parseLibCPER(ContentType::CPER, memoryCper);
    ASSERT_EQ(result.sectionGuids.size(), 1);

    bool called = false;
    addOem(result.sectionGuids.front(),
           [&](v1::Type type, std::span<const std::uint8_t> raw,
               const v1::Oem& base,
               std::span<const v1::Guid> guids) -> v1::Entry {
               called = true;
               EXPECT_EQ(type, v1::Type::CPER);
               EXPECT_EQ(raw.size(), memoryCper.size());
               EXPECT_TRUE(base.contains("OpenBMC"));
               if (guids.size() != 1)
               {
                   ADD_FAILURE() << "Expected one matched GUID";
                   return {.key = "", .value = {}};
               }
               EXPECT_EQ(guids.front(), result.sectionGuids.front());
               return {.key = "Vendor", .value = {{"field", 1}}};
           });
    runOem(ContentType::CPER, memoryCper, result);

    EXPECT_TRUE(called);
    ASSERT_TRUE(result.oem.contains("Vendor"));
    EXPECT_EQ(nlohmann::json::parse(result.oem.at("Vendor")).at("field"), 1);
    // Base entry untouched.
    EXPECT_TRUE(result.oem.contains("OpenBMC"));
}

TEST_F(DecoderTest, OemSkipsUnmatchedGuids)
{
    auto result = parseLibCPER(ContentType::CPER, memoryCper);
    addOem("ffffffff-ffff-ffff-ffff-ffffffffffff",
           [](v1::Type, std::span<const std::uint8_t>, const v1::Oem&,
              std::span<const v1::Guid>) -> v1::Entry {
               ADD_FAILURE() << "Unmatched handler must not run";
               return {.key = "Nope", .value = {}};
           });
    runOem(ContentType::CPER, memoryCper, result);
    EXPECT_EQ(result.oem.size(), 1);
}

TEST_F(DecoderTest, OemFailureSkipsEntry)
{
    auto result = parseLibCPER(ContentType::CPER, memoryCper);
    addOem(result.sectionGuids.front(),
           [](v1::Type, std::span<const std::uint8_t>, const v1::Oem&,
              std::span<const v1::Guid>) -> v1::Entry {
               throw std::runtime_error("decode blew up");
           });
    EXPECT_NO_THROW(runOem(ContentType::CPER, memoryCper, result));
    EXPECT_EQ(result.oem.size(), 1);
}

TEST_F(DecoderTest, OemRejectsReservedAndDuplicateKeys)
{
    auto result = parseLibCPER(ContentType::CPER, memoryCper);
    // Empty, reserved, and duplicate keys are skipped.
    addOem(result.sectionGuids.front(),
           [](v1::Type, std::span<const std::uint8_t>, const v1::Oem&,
              std::span<const v1::Guid>) -> v1::Entry {
               return {.key = "", .value = {}};
           });
    runOem(ContentType::CPER, memoryCper, result);
    EXPECT_EQ(result.oem.size(), 1);

    // Duplicate keys across plugins: first wins.
    auto result2 = parseLibCPER(ContentType::CPER, memoryCper);
    addOem("aaaaaaaa-0000-0000-0000-000000000000",
           [](v1::Type, std::span<const std::uint8_t>, const v1::Oem&,
              std::span<const v1::Guid>) -> v1::Entry {
               return {.key = "Dup", .value = {{"n", 1}}};
           });
    result2.sectionGuids.push_back("aaaaaaaa-0000-0000-0000-000000000000");
    runOem(ContentType::CPER, memoryCper, result2);
    ASSERT_TRUE(result2.oem.contains("Dup"));
    EXPECT_EQ(nlohmann::json::parse(result2.oem.at("Dup")).at("n"), 1);
}

TEST_F(DecoderTest, OemDuplicateGuidFirstWins)
{
    auto result = parseLibCPER(ContentType::CPER, memoryCper);
    addOem(result.sectionGuids.front(),
           [](v1::Type, std::span<const std::uint8_t>, const v1::Oem&,
              std::span<const v1::Guid>) -> v1::Entry {
               return {.key = "First", .value = {}};
           });
    addOem(result.sectionGuids.front(),
           [](v1::Type, std::span<const std::uint8_t>, const v1::Oem&,
              std::span<const v1::Guid>) -> v1::Entry {
               return {.key = "Second", .value = {}};
           });
    runOem(ContentType::CPER, memoryCper, result);
    EXPECT_TRUE(result.oem.contains("First"));
    EXPECT_FALSE(result.oem.contains("Second"));
}

TEST_F(DecoderTest, OemPluginLoadsFromDirectory)
{
    auto dir = loadTestPlugin();
    auto result = parseLibCPER(ContentType::CPER, memoryCper);
    runOem(ContentType::CPER, memoryCper, result);

    // The fixture plugin decodes the memory section GUID.
    ASSERT_TRUE(result.oem.contains("TestOem")) << dir.string();
    const auto entry = nlohmann::json::parse(result.oem.at("TestOem"));
    EXPECT_EQ(entry.at("rawSize"), memoryCper.size());
    EXPECT_EQ(entry.at("matched"), 1);
    EXPECT_EQ(entry.at("baseKeys"), 1);
    std::filesystem::remove_all(dir);
}

TEST_F(DecoderTest, OemPluginMergesIntoEvent)
{
    auto dir = loadTestPlugin();
    auto json = runProcess(
        sdbusplus::object_path{"/xyz/openbmc_project/inventory/DIMM_0"},
        ContentType::CPER,
        std::vector<std::uint8_t>(std::from_range, memoryCper));

    const auto& processed = json.at("xyz.openbmc_project.State.CPER.Corrected")
                                .at("_EXTENSIONS")
                                .at("xyz.openbmc_project.Logging.Extension."
                                    "CPER.Processed");
    ASSERT_TRUE(processed.at("Oem").contains("TestOem")) << dir.string();
    std::filesystem::remove_all(dir);
}

} // namespace phosphor::logging::cper::test
