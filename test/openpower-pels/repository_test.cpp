#include "extensions/openpower-pels/paths.hpp"
#include "extensions/openpower-pels/repository.hpp"
#include "pel_utils.hpp"

#include <ext/stdio_filebuf.h>

#include <filesystem>

#include <gtest/gtest.h>

using namespace openpower::pels;
namespace fs = std::filesystem;

/**
 * Clean the Repo after every testcase.
 * And because we have PEL object, also clean up
 * the log ID.
 */
class RepositoryTest : public CleanLogID
{
  protected:
    void SetUp() override
    {
        repoPath = getPELRepoPath();
    }

    void TearDown() override
    {
        fs::remove_all(repoPath);
    }

    fs::path repoPath;
};

TEST_F(RepositoryTest, FilenameTest)
{
    BCDTime date = {0x20, 0x30, 0x11, 0x28, 0x13, 0x6, 0x7, 0x8};

    EXPECT_EQ(Repository::getPELFilename(0x12345678, date),
              "2030112813060708_12345678");

    EXPECT_EQ(Repository::getPELFilename(0xAABBCCDD, date),
              "2030112813060708_AABBCCDD");

    EXPECT_EQ(Repository::getPELFilename(0x3AFF1, date),
              "2030112813060708_0003AFF1");

    EXPECT_EQ(Repository::getPELFilename(100, date),
              "2030112813060708_00000064");

    EXPECT_EQ(Repository::getPELFilename(0, date), "2030112813060708_00000000");
}

TEST_F(RepositoryTest, AddTest)
{
    Repository repo{repoPath};
    auto data = pelDataFactory(TestPelType::pelSimple);
    auto pel = std::make_unique<PEL>(*data);

    repo.add(pel);

    // Check that the PEL was stored where it was supposed to be,
    // and that it wrote the PEL data.
    const auto& ts = pel->privateHeader()->commitTimestamp();
    auto name = Repository::getPELFilename(pel->id(), ts);

    fs::path file = repoPath / "logs" / name;
    EXPECT_TRUE(fs::exists(file));

    auto newData = readPELFile(file);
    auto pelData = pel->data();
    EXPECT_EQ(*newData, pelData);
}

TEST_F(RepositoryTest, RestoreTest)
{
    using pelID = Repository::LogID::Pel;
    using obmcID = Repository::LogID::Obmc;

    std::vector<Repository::LogID> ids;

    {
        Repository repo{repoPath};

        // Add some PELs to the repository
        {
            auto data = pelDataFactory(TestPelType::pelSimple);
            auto pel = std::make_unique<PEL>(*data, 1);
            pel->assignID();
            repo.add(pel);
            ids.emplace_back(pelID(pel->id()), obmcID(1));
        }
        {
            auto data = pelDataFactory(TestPelType::pelSimple);
            auto pel = std::make_unique<PEL>(*data, 2);
            pel->assignID();
            repo.add(pel);
            ids.emplace_back(pelID(pel->id()), obmcID(2));
        }

        // Check they're there
        EXPECT_TRUE(repo.hasPEL(ids[0]));
        EXPECT_TRUE(repo.hasPEL(ids[1]));

        // Do some other search tests while we're here.

        // Search based on PEL ID
        Repository::LogID id(pelID(ids[0].pelID));
        EXPECT_TRUE(repo.hasPEL(id));

        // Search based on OBMC log ID
        id.pelID.id = 0;
        id.obmcID = ids[0].obmcID;
        EXPECT_TRUE(repo.hasPEL(id));

        // ... based on the other PEL ID
        id.pelID = ids[1].pelID;
        id.obmcID.id = 0;
        EXPECT_TRUE(repo.hasPEL(id));

        // Not found
        id.pelID.id = 99;
        id.obmcID.id = 100;
        EXPECT_FALSE(repo.hasPEL(id));
    }

    {
        // Restore and check they're still there, then
        // remove them.
        Repository repo{repoPath};
        EXPECT_TRUE(repo.hasPEL(ids[0]));
        EXPECT_TRUE(repo.hasPEL(ids[1]));

        repo.remove(ids[0]);
        EXPECT_FALSE(repo.hasPEL(ids[0]));

        repo.remove(ids[1]);
        EXPECT_FALSE(repo.hasPEL(ids[1]));
    }
}
