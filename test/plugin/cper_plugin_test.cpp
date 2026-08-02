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

} // namespace

TEST(CperPluginTest, ArtifactPathGeneration)
{
    const auto root = testArtifactRoot();

    EXPECT_EQ(artifactPath("/xyz/openbmc_project/logging/entry/42", root),
              root / "42.bin");
}

TEST(CperPluginTest, PersistArtifact)
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

    std::filesystem::remove_all(root);
}

TEST(CperPluginTest, PersistArtifactMissingFd)
{
    auto path = persistArtifact("/xyz/openbmc_project/logging/entry/42", -1,
                                testArtifactRoot());

    EXPECT_TRUE(path.empty());
}

TEST(CperPluginTest, PersistArtifactClosedFd)
{
    int fd = ::open("/dev/null", O_RDONLY);
    ASSERT_GE(fd, 0);

    ::close(fd);

    auto path = persistArtifact("/xyz/openbmc_project/logging/entry/42", fd,
                                testArtifactRoot());

    EXPECT_TRUE(path.empty());
}

TEST(CperPluginTest, RegistrationLookup)
{
    PluginRegistry registry;

    registry.registerPlugin(cper::interface,
                            std::make_unique<Factory>(testArtifactRoot()));

    auto factory = registry.lookup(cper::interface);

    ASSERT_NE(factory, nullptr);
}

TEST(CperPluginTest, FactoryCreate)
{
    TemporaryArtifact artifact;

    Factory factory(testArtifactRoot());

    auto bus = sdbusplus::bus::new_default();

    PluginContext context{
        bus,
        "/xyz/openbmc_project/logging/entry/1",
    };

    Descriptor descriptor(DiagnosticDataType::CPER, "notification-guid",
                          "section-guid", artifact.getFd());

    auto plugin = factory.create(context, descriptor);

    ASSERT_NE(plugin, nullptr);

    EXPECT_EQ(plugin->interface(), plugin::cper::interface);

    EXPECT_TRUE(std::filesystem::exists(testArtifactRoot() / "1.bin"));

    std::filesystem::remove_all(testArtifactRoot());
}

TEST(CperPluginTest, RegisterAndCreatePlugin)
{
    TemporaryArtifact artifact;

    PluginRegistry registry;

    registry.registerPlugin(cper::interface,
                            std::make_unique<Factory>(testArtifactRoot()));

    PluginManager manager(registry);

    auto bus = sdbusplus::bus::new_default();

    PluginContext context{
        bus,
        "/xyz/openbmc_project/logging/entry/1",
    };

    Descriptor descriptor(DiagnosticDataType::CPER, "notification-guid",
                          "section-guid", artifact.getFd());

    auto plugin = manager.create(context, descriptor);

    ASSERT_NE(plugin, nullptr);

    EXPECT_EQ(plugin->interface(), plugin::cper::interface);

    EXPECT_TRUE(std::filesystem::exists(testArtifactRoot() / "1.bin"));

    std::filesystem::remove_all(testArtifactRoot());
}

TEST(CperPluginTest, CreatePluginWithCperSectionType)
{
    TemporaryArtifact artifact;

    PluginRegistry registry;

    registry.registerPlugin(cper::interface,
                            std::make_unique<Factory>(testArtifactRoot()));

    PluginManager manager(registry);

    auto bus = sdbusplus::bus::new_default();

    PluginContext context{
        bus,
        "/xyz/openbmc_project/logging/entry/1",
    };

    Descriptor descriptor(DiagnosticDataType::CPERSection, "notification-guid",
                          "section-guid", artifact.getFd());

    auto plugin = manager.create(context, descriptor);

    ASSERT_NE(plugin, nullptr);

    EXPECT_EQ(plugin->interface(), plugin::cper::interface);

    EXPECT_TRUE(std::filesystem::exists(testArtifactRoot() / "1.bin"));

    std::filesystem::remove_all(testArtifactRoot());
}

TEST(CperPluginTest, GetCPERBinaryReturnsPersistedArtifact)
{
    TemporaryArtifact artifact;

    Factory factory(testArtifactRoot());

    auto bus = sdbusplus::bus::new_default();

    PluginContext context{
        bus,
        "/xyz/openbmc_project/logging/entry/99",
    };

    Descriptor descriptor(DiagnosticDataType::CPER, "notification-guid",
                          "section-guid", artifact.getFd());

    auto plugin = factory.create(context, descriptor);

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

    std::filesystem::remove_all(testArtifactRoot());
}

} // namespace phosphor::logging::plugin::cper
