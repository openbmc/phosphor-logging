/**
 * Copyright © 2019 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
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
    auto data = pelDataFactory(TestPELType::pelSimple);
    auto pel = std::make_unique<PEL>(data);

    repo.add(pel);

    // Check that the PEL was stored where it was supposed to be,
    // and that it wrote the PEL data.
    const auto ts = pel->privateHeader().commitTimestamp();
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
            auto data = pelDataFactory(TestPELType::pelSimple);
            auto pel = std::make_unique<PEL>(data, 1);
            pel->assignID();
            repo.add(pel);
            ids.emplace_back(pelID(pel->id()), obmcID(1));
        }
        {
            auto data = pelDataFactory(TestPELType::pelSimple);
            auto pel = std::make_unique<PEL>(data, 2);
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

TEST_F(RepositoryTest, TestGetPELData)
{
    using ID = Repository::LogID;
    Repository repo{repoPath};

    ID badID{ID::Pel(42)};
    auto noData = repo.getPELData(badID);
    EXPECT_FALSE(noData);

    // Add a PEL to the repo, and get the data back with getPELData.
    auto data = pelDataFactory(TestPELType::pelSimple);
    auto dataCopy = data;
    auto pel = std::make_unique<PEL>(data);
    auto pelID = pel->id();
    repo.add(pel);

    ID id{ID::Pel(pelID)};
    auto pelData = repo.getPELData(id);

    ASSERT_TRUE(pelData);
    EXPECT_EQ(dataCopy, *pelData);
}

TEST_F(RepositoryTest, TestForEach)
{
    Repository repo{repoPath};

    // Add 2 PELs
    auto data = pelDataFactory(TestPELType::pelSimple);
    auto pel = std::make_unique<PEL>(data);
    repo.add(pel);

    pel = std::make_unique<PEL>(data);
    pel->assignID();
    pel->setCommitTime();
    repo.add(pel);

    // Make a function that saves the IDs
    std::vector<uint32_t> ids;
    Repository::ForEachFunc f1 = [&ids](const PEL& pel) {
        ids.push_back(pel.id());
        return false;
    };

    repo.for_each(f1);

    EXPECT_EQ(ids.size(), 2);

    // Stop after the first time in.
    Repository::ForEachFunc f2 = [&ids](const PEL& pel) {
        ids.push_back(pel.id());
        return true;
    };

    ids.clear();
    repo.for_each(f2);
    EXPECT_EQ(ids.size(), 1);
}

TEST_F(RepositoryTest, TestSubscriptions)
{
    std::vector<uint32_t> added;
    std::vector<uint32_t> removed;

    Repository::AddCallback ac = [&added](const PEL& pel) {
        added.push_back(pel.id());
    };

    Repository::DeleteCallback dc = [&removed](uint32_t id) {
        removed.push_back(id);
    };

    Repository repo{repoPath};
    repo.subscribeToAdds("test", ac);
    repo.subscribeToDeletes("test", dc);

    auto data = pelDataFactory(TestPELType::pelSimple);
    auto pel = std::make_unique<PEL>(data);
    auto pelID = pel->id();
    repo.add(pel);

    EXPECT_EQ(added.size(), 1);

    using ID = Repository::LogID;
    ID id{ID::Pel(pelID)};
    repo.remove(id);

    EXPECT_EQ(removed.size(), 1);

    repo.unsubscribeFromAdds("test");
    repo.unsubscribeFromDeletes("test");

    added.clear();
    removed.clear();

    repo.add(pel);
    EXPECT_EQ(added.size(), 0);

    repo.remove(id);
    EXPECT_EQ(removed.size(), 0);
}

TEST_F(RepositoryTest, TestGetAttributes)
{
    uint32_t pelID = 0;
    std::bitset<16> actionFlags;

    {
        Repository repo{repoPath};

        // Add a PEL to the repo
        auto data = pelDataFactory(TestPELType::pelSimple);
        auto pel = std::make_unique<PEL>(data);
        repo.add(pel);

        pelID = pel->id();
        actionFlags = pel->userHeader().actionFlags();

        using ID = Repository::LogID;
        ID id{ID::Pel(pelID)};

        auto a = repo.getPELAttributes(id);
        EXPECT_TRUE(a);
        EXPECT_EQ((*a).get().actionFlags, actionFlags);

        id.pelID.id = 0;
        a = repo.getPELAttributes(id);
        EXPECT_FALSE(a);
    }

    {
        // Restore the repository and check again
        Repository repo{repoPath};

        using ID = Repository::LogID;
        ID id{ID::Pel(pelID)};

        auto a = repo.getPELAttributes(id);
        EXPECT_TRUE(a);
        EXPECT_EQ((*a).get().actionFlags, actionFlags);

        id.pelID.id = 0;
        a = repo.getPELAttributes(id);
        EXPECT_FALSE(a);
    }
}

TEST_F(RepositoryTest, TestSetHostState)
{
    // Add a PEL to the repo
    auto data = pelDataFactory(TestPELType::pelSimple);
    auto pel = std::make_unique<PEL>(data);
    using ID = Repository::LogID;
    ID id{ID::Pel(pel->id())};

    {
        Repository repo{repoPath};

        repo.add(pel);

        auto a = repo.getPELAttributes(id);
        EXPECT_EQ((*a).get().hostState, TransmissionState::newPEL);

        repo.setPELHostTransState(pel->id(), TransmissionState::acked);

        // First, check the attributes
        a = repo.getPELAttributes(id);
        EXPECT_EQ((*a).get().hostState, TransmissionState::acked);

        // Next, check the PEL data itself
        auto pelData = repo.getPELData(id);
        PEL newPEL{*pelData};
        EXPECT_EQ(newPEL.hostTransmissionState(), TransmissionState::acked);
    }

    {
        // Now restore, and check again
        Repository repo{repoPath};

        // First, check the attributes
        auto a = repo.getPELAttributes(id);
        EXPECT_EQ((*a).get().hostState, TransmissionState::acked);

        // Next, check the PEL data itself
        auto pelData = repo.getPELData(id);
        PEL newPEL{*pelData};
        EXPECT_EQ(newPEL.hostTransmissionState(), TransmissionState::acked);
    }
}

TEST_F(RepositoryTest, TestSetHMCState)
{
    // Add a PEL to the repo
    auto data = pelDataFactory(TestPELType::pelSimple);
    auto pel = std::make_unique<PEL>(data);
    using ID = Repository::LogID;
    ID id{ID::Pel(pel->id())};

    {
        Repository repo{repoPath};

        repo.add(pel);

        auto a = repo.getPELAttributes(id);
        EXPECT_EQ((*a).get().hmcState, TransmissionState::newPEL);

        repo.setPELHMCTransState(pel->id(), TransmissionState::acked);

        // First, check the attributes
        a = repo.getPELAttributes(id);
        EXPECT_EQ((*a).get().hmcState, TransmissionState::acked);

        // Next, check the PEL data itself
        auto pelData = repo.getPELData(id);
        PEL newPEL{*pelData};
        EXPECT_EQ(newPEL.hmcTransmissionState(), TransmissionState::acked);
    }

    {
        // Now restore, and check again
        Repository repo{repoPath};

        // First, check the attributes
        auto a = repo.getPELAttributes(id);
        EXPECT_EQ((*a).get().hmcState, TransmissionState::acked);

        // Next, check the PEL data itself
        auto pelData = repo.getPELData(id);
        PEL newPEL{*pelData};
        EXPECT_EQ(newPEL.hmcTransmissionState(), TransmissionState::acked);
    }
}

TEST_F(RepositoryTest, TestGetPELFD)
{
    Repository repo{repoPath};

    auto data = pelDataFactory(TestPELType::pelSimple);
    auto pel = std::make_unique<PEL>(data);
    pel->setCommitTime();
    pel->assignID();

    repo.add(pel);

    using ID = Repository::LogID;
    ID id{ID::Pel(pel->id())};

    auto fd = repo.getPELFD(id);

    EXPECT_TRUE(fd);

    // Get the size
    struct stat s;
    int r = fstat(*fd, &s);
    ASSERT_EQ(r, 0);

    auto size = s.st_size;

    // Read the PEL data out of the FD
    FILE* fp = fdopen(*fd, "r");
    ASSERT_NE(fp, nullptr);

    std::vector<uint8_t> newData;
    newData.resize(size);
    r = fread(newData.data(), 1, size, fp);
    EXPECT_EQ(r, size);

    PEL newPEL{newData};

    EXPECT_TRUE(newPEL.valid());
    EXPECT_EQ(newPEL.id(), pel->id());

    fclose(fp);

    // Call getPELFD again, this time with a bad ID
    id.pelID.id = 42;
    fd = repo.getPELFD(id);

    EXPECT_FALSE(fd);
}
