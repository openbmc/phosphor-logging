#include "plugin/cper_artifact.hpp"
#include "plugin/cper_descriptor.hpp"
#include "plugin/cper_plugin.hpp"
#include "plugin/plugin_manager.hpp"
#include "plugin/plugin_registry.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <array>
#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>

#include <gtest/gtest.h>

namespace phosphor::logging::plugin::cper
{

namespace
{

constexpr auto testPayload = "CPER test payload";

std::filesystem::path testArtifactRoot()
{
    return std::filesystem::temp_directory_path() /
           "phosphor-logging-cper-test";
}

class TemporaryArtifact
{
  public:
    TemporaryArtifact()
    {
        path = std::filesystem::temp_directory_path() / "cper-test.bin";

        std::ofstream out(path);

        if (!out.good())
        {
            throw std::runtime_error("Failed to create test artifact");
        }

        out << testPayload;
        out.close();

        fd = ::open(path.c_str(), O_RDONLY);

        if (fd < 0)
        {
            throw std::system_error(errno, std::generic_category(),
                                    "Failed to open test artifact");
        }
    }

    TemporaryArtifact(const TemporaryArtifact&) = delete;
    TemporaryArtifact& operator=(const TemporaryArtifact&) = delete;
    TemporaryArtifact(TemporaryArtifact&&) = delete;
    TemporaryArtifact& operator=(TemporaryArtifact&&) = delete;

    ~TemporaryArtifact()
    {
        if (fd >= 0)
        {
            ::close(fd);
        }

        std::error_code ec;
        std::filesystem::remove(path, ec);
    }

    int getFd() const
    {
        return fd;
    }

  private:
    std::filesystem::path path;
    int fd = -1;
};

class CperPluginTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        bus = std::make_unique<sdbusplus::bus_t>(sdbusplus::bus::new_default());
    }

    void TearDown() override
    {
        std::error_code ec;
        std::filesystem::remove_all(testArtifactRoot(), ec);
    }

    PluginContext makeContext(
        const std::string& path = "/xyz/openbmc_project/logging/entry/1")
    {
        return PluginContext{
            *bus,
            path,
        };
    }

    static Descriptor makeDescriptor(DiagnosticDataType type, int fd)
    {
        return Descriptor(type, "notification-guid", "section-guid", fd);
    }

    std::unique_ptr<sdbusplus::bus_t> bus;
};

} // namespace

TEST_F(CperPluginTest, ArtifactPathGeneration)
{
    const auto root = testArtifactRoot();

    EXPECT_EQ(artifactPath("/xyz/openbmc_project/logging/entry/42", root),
              root / "42.bin");
}

TEST_F(CperPluginTest, PersistArtifact)
{
    TemporaryArtifact source;

    const auto root = testArtifactRoot();

    const auto artifact = persistArtifact(
        "/xyz/openbmc_project/logging/entry/42", source.getFd(), root);

    EXPECT_TRUE(std::filesystem::exists(artifact));

    std::ifstream in(artifact);

    ASSERT_TRUE(in.good());

    std::string content;
    std::getline(in, content);

    EXPECT_EQ(content, testPayload);
}

TEST_F(CperPluginTest, PersistArtifactMissingFd)
{
    auto path = persistArtifact("/xyz/openbmc_project/logging/entry/42", -1,
                                testArtifactRoot());

    EXPECT_TRUE(path.empty());
}

TEST_F(CperPluginTest, PersistArtifactClosedFd)
{
    int fd = ::open("/dev/null", O_RDONLY);

    ASSERT_GE(fd, 0);

    ::close(fd);

    auto path = persistArtifact("/xyz/openbmc_project/logging/entry/42", fd,
                                testArtifactRoot());

    EXPECT_TRUE(path.empty());
}

TEST_F(CperPluginTest, RegistrationLookup)
{
    PluginRegistry registry;

    registry.registerPlugin(cper::interface,
                            std::make_unique<Factory>(testArtifactRoot()));

    auto factory = registry.lookup(cper::interface);

    ASSERT_NE(factory, nullptr);
}

TEST_F(CperPluginTest, FactoryCreate)
{
    TemporaryArtifact artifact;

    Factory factory(testArtifactRoot());

    auto plugin = factory.create(
        makeContext(),
        makeDescriptor(DiagnosticDataType::CPER, artifact.getFd()));

    ASSERT_NE(plugin, nullptr);

    EXPECT_EQ(plugin->interface(), plugin::cper::interface);

    EXPECT_TRUE(std::filesystem::exists(testArtifactRoot() / "1.bin"));
}

TEST_F(CperPluginTest, RegisterAndCreatePlugin)
{
    TemporaryArtifact artifact;

    PluginRegistry registry;

    registry.registerPlugin(cper::interface,
                            std::make_unique<Factory>(testArtifactRoot()));

    PluginManager manager(registry);

    auto plugin = manager.create(
        makeContext(),
        makeDescriptor(DiagnosticDataType::CPER, artifact.getFd()));

    ASSERT_NE(plugin, nullptr);

    EXPECT_EQ(plugin->interface(), plugin::cper::interface);

    EXPECT_TRUE(std::filesystem::exists(testArtifactRoot() / "1.bin"));
}

TEST_F(CperPluginTest, CreatePluginWithCperSectionType)
{
    TemporaryArtifact artifact;

    PluginRegistry registry;

    registry.registerPlugin(cper::interface,
                            std::make_unique<Factory>(testArtifactRoot()));

    PluginManager manager(registry);

    auto plugin = manager.create(
        makeContext(),
        makeDescriptor(DiagnosticDataType::CPERSection, artifact.getFd()));

    ASSERT_NE(plugin, nullptr);

    EXPECT_EQ(plugin->interface(), plugin::cper::interface);

    EXPECT_TRUE(std::filesystem::exists(testArtifactRoot() / "1.bin"));
}

TEST_F(CperPluginTest, GetCPERBinaryReturnsPersistedArtifact)
{
    TemporaryArtifact artifact;

    Factory factory(testArtifactRoot());

    auto plugin = factory.create(
        makeContext("/xyz/openbmc_project/logging/entry/99"),
        makeDescriptor(DiagnosticDataType::CPER, artifact.getFd()));

    ASSERT_NE(plugin, nullptr);

    auto* cperPlugin = dynamic_cast<Plugin*>(plugin.get());

    ASSERT_NE(cperPlugin, nullptr);

    auto fd = cperPlugin->getCPERBinary();

    std::array<char, 128> buffer{};

    const auto bytesRead = ::read(fd, buffer.data(), buffer.size());

    ASSERT_GT(bytesRead, 0);

    std::string content(buffer.data(), static_cast<std::size_t>(bytesRead));

    EXPECT_EQ(content, testPayload);

    ::close(fd);
}

TEST_F(CperPluginTest, BuildPayload)
{
    Factory factory;

    nlohmann::json metadata{
        {"AMD",
         {
             {"AEL.VERSION", "1.0"},
             {"AEL.AFID", "4660"},
         }},
    };

    auto payload = factory.buildPayload(metadata);

    ASSERT_TRUE(payload.contains(oemKey));

    EXPECT_EQ(payload[oemKey]["AMD"], metadata["AMD"].dump());
}

TEST_F(CperPluginTest, BuildPayloadEmptyMetadata)
{
    Factory factory;

    auto payload = factory.buildPayload(nlohmann::json::object());

    ASSERT_TRUE(payload.contains(oemKey));

    EXPECT_TRUE(payload[oemKey].empty());
}

} // namespace phosphor::logging::plugin::cper
