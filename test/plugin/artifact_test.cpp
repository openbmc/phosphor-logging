#include "plugin/utils/artifact.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <filesystem>
#include <vector>

#include <gtest/gtest.h>

namespace phosphor::logging::plugin::utils
{

class ArtifactTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        root = std::filesystem::temp_directory_path() /
               "phosphor-logging-artifact-test";

        std::filesystem::create_directories(root);
    }

    void TearDown() override
    {
        std::error_code ec;
        std::filesystem::remove_all(root, ec);
    }

    std::filesystem::path root;
};

TEST_F(ArtifactTest, PersistEmptyPayload)
{
    auto artifact = persistArtifact({}, "artifact", root);

    EXPECT_FALSE(artifact);
}

TEST_F(ArtifactTest, PersistArtifact)
{
    const std::vector<uint8_t> data{0x01, 0x02, 0x03, 0x04};

    auto artifact = persistArtifact(data, "artifact", root);

    ASSERT_TRUE(artifact);

    EXPECT_TRUE(std::filesystem::exists(artifact.path));
}

TEST_F(ArtifactTest, OpenArtifact)
{
    const std::vector<uint8_t> data{0x11, 0x22, 0x33, 0x44};

    auto artifact = persistArtifact(data, "artifact", root);

    ASSERT_TRUE(artifact);

    auto fd = openArtifact(artifact);

    ASSERT_GE(fd, 0);

    std::vector<uint8_t> readBack(data.size());

    auto bytes = ::read(fd, readBack.data(), readBack.size());

    EXPECT_EQ(bytes, static_cast<ssize_t>(data.size()));

    EXPECT_EQ(data, readBack);

    ::close(fd);
}

TEST_F(ArtifactTest, OpenInvalidArtifact)
{
    ArtifactRef artifact{.path = "/tmp/does-not-exist"};

    EXPECT_EQ(openArtifact(artifact), -1);
}

TEST_F(ArtifactTest, RemoveArtifact)
{
    const std::vector<uint8_t> data{0x01, 0x02};

    auto artifact = persistArtifact(data, "artifact", root);

    ASSERT_TRUE(artifact);

    EXPECT_TRUE(std::filesystem::exists(artifact.path));

    removeArtifact(artifact);

    EXPECT_FALSE(std::filesystem::exists(artifact.path));
}

TEST_F(ArtifactTest, RemoveMissingArtifact)
{
    ArtifactRef artifact{.path = "/tmp/does-not-exist"};

    EXPECT_NO_THROW(removeArtifact(artifact));
}

TEST_F(ArtifactTest, PayloadExceedsMaximumSize)
{
    std::vector<uint8_t> data(maxArtifactPayloadSize + 1, 0xAA);

    auto artifact = persistArtifact(data, "artifact", root);

    EXPECT_FALSE(artifact);
}

TEST_F(ArtifactTest, ArtifactNamesAreUnique)
{
    const std::vector<uint8_t> data{0x01, 0x02, 0x03};

    auto first = persistArtifact(data, "artifact", root);

    auto second = persistArtifact(data, "artifact", root);

    ASSERT_TRUE(first);
    ASSERT_TRUE(second);

    EXPECT_NE(first.path, second.path);
}

} // namespace phosphor::logging::plugin::utils
