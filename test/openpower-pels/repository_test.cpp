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

// Test the repo size statistics
TEST_F(RepositoryTest, TestRepoSizes)
{
    uint32_t id = 1;

    {
        Repository repo{repoPath, 10000};

        // All sizes are zero
        {
            const auto& stats = repo.getSizeStats();
            EXPECT_EQ(stats.total, 0);
            EXPECT_EQ(stats.bmc, 0);
            EXPECT_EQ(stats.nonBMC, 0);
            EXPECT_EQ(stats.bmcServiceable, 0);
            EXPECT_EQ(stats.bmcInfo, 0);
            EXPECT_EQ(stats.nonBMCServiceable, 0);
            EXPECT_EQ(stats.nonBMCInfo, 0);
        }

        // Add a 2000B BMC predictive error
        auto data = pelFactory(id++, 'O', 0x20, 0x8800, 2000);
        auto pel = std::make_unique<PEL>(data);
        auto pelID1 = pel->id();
        repo.add(pel);

        {
            const auto& stats = repo.getSizeStats();
            EXPECT_EQ(stats.total, 2000);
            EXPECT_EQ(stats.bmc, 2000);
            EXPECT_EQ(stats.nonBMC, 0);
            EXPECT_EQ(stats.bmcServiceable, 2000);
            EXPECT_EQ(stats.bmcInfo, 0);
            EXPECT_EQ(stats.nonBMCServiceable, 0);
            EXPECT_EQ(stats.nonBMCInfo, 0);
        }

        // Add a 1000B BMC informational error
        data = pelFactory(id++, 'O', 0x00, 0x8800, 1000);
        pel = std::make_unique<PEL>(data);
        auto pelID2 = pel->id();
        repo.add(pel);

        {
            const auto& stats = repo.getSizeStats();
            EXPECT_EQ(stats.total, 3000);
            EXPECT_EQ(stats.bmc, 3000);
            EXPECT_EQ(stats.nonBMC, 0);
            EXPECT_EQ(stats.bmcServiceable, 2000);
            EXPECT_EQ(stats.bmcInfo, 1000);
            EXPECT_EQ(stats.nonBMCServiceable, 0);
            EXPECT_EQ(stats.nonBMCInfo, 0);
        }

        // Add a 4000B Hostboot unrecoverable error
        data = pelFactory(id++, 'B', 0x40, 0x8800, 4000);
        pel = std::make_unique<PEL>(data);
        auto pelID3 = pel->id();
        repo.add(pel);

        {
            const auto& stats = repo.getSizeStats();
            EXPECT_EQ(stats.total, 7000);
            EXPECT_EQ(stats.bmc, 3000);
            EXPECT_EQ(stats.nonBMC, 4000);
            EXPECT_EQ(stats.bmcServiceable, 2000);
            EXPECT_EQ(stats.bmcInfo, 1000);
            EXPECT_EQ(stats.nonBMCServiceable, 4000);
            EXPECT_EQ(stats.nonBMCInfo, 0);
        }

        // Add a 5000B Hostboot informational error
        data = pelFactory(id++, 'B', 0x00, 0x8800, 5000);
        pel = std::make_unique<PEL>(data);
        auto pelID4 = pel->id();
        repo.add(pel);

        {
            const auto& stats = repo.getSizeStats();
            EXPECT_EQ(stats.total, 12000);
            EXPECT_EQ(stats.bmc, 3000);
            EXPECT_EQ(stats.nonBMC, 9000);
            EXPECT_EQ(stats.bmcServiceable, 2000);
            EXPECT_EQ(stats.bmcInfo, 1000);
            EXPECT_EQ(stats.nonBMCServiceable, 4000);
            EXPECT_EQ(stats.nonBMCInfo, 5000);
        }

        // Remove the BMC serviceable error
        using ID = Repository::LogID;
        ID id1{ID::Pel(pelID1)};

        repo.remove(id1);
        {
            const auto& stats = repo.getSizeStats();
            EXPECT_EQ(stats.total, 10000);
            EXPECT_EQ(stats.bmc, 1000);
            EXPECT_EQ(stats.nonBMC, 9000);
            EXPECT_EQ(stats.bmcServiceable, 0);
            EXPECT_EQ(stats.bmcInfo, 1000);
            EXPECT_EQ(stats.nonBMCServiceable, 4000);
            EXPECT_EQ(stats.nonBMCInfo, 5000);
        }

        // Remove the Hostboot informational error
        ID id4{ID::Pel(pelID4)};

        repo.remove(id4);
        {
            const auto& stats = repo.getSizeStats();
            EXPECT_EQ(stats.total, 5000);
            EXPECT_EQ(stats.bmc, 1000);
            EXPECT_EQ(stats.nonBMC, 4000);
            EXPECT_EQ(stats.bmcServiceable, 0);
            EXPECT_EQ(stats.bmcInfo, 1000);
            EXPECT_EQ(stats.nonBMCServiceable, 4000);
            EXPECT_EQ(stats.nonBMCInfo, 0);
        }

        // Remove the BMC informational error
        ID id2{ID::Pel(pelID2)};

        repo.remove(id2);
        {
            const auto& stats = repo.getSizeStats();
            EXPECT_EQ(stats.total, 4000);
            EXPECT_EQ(stats.bmc, 0);
            EXPECT_EQ(stats.nonBMC, 4000);
            EXPECT_EQ(stats.bmcServiceable, 0);
            EXPECT_EQ(stats.bmcInfo, 0);
            EXPECT_EQ(stats.nonBMCServiceable, 4000);
            EXPECT_EQ(stats.nonBMCInfo, 0);
        }

        // Remove the hostboot unrecoverable error
        ID id3{ID::Pel(pelID3)};

        repo.remove(id3);
        {
            const auto& stats = repo.getSizeStats();
            EXPECT_EQ(stats.total, 0);
            EXPECT_EQ(stats.bmc, 0);
            EXPECT_EQ(stats.nonBMC, 0);
            EXPECT_EQ(stats.bmcServiceable, 0);
            EXPECT_EQ(stats.bmcInfo, 0);
            EXPECT_EQ(stats.nonBMCServiceable, 0);
            EXPECT_EQ(stats.nonBMCInfo, 0);
        }
    }
}
// Prune PELs, when no HMC/OS/PHYP acks
TEST_F(RepositoryTest, TestPruneNoAcks)
{
    uint32_t id = 1;
    Repository repo{repoPath, 10000};

    // Add 5K each of BMC nonInfo, Info and nonBMC info, nonInfo errors.
    // None of them acked by PHYP, host, or HMC for a total of 20KB.
    for (int i = 0; i < 10; i++)
    {
        // BMC predictive
        auto data = pelFactory(id++, 'O', 0x20, 0x8800, 500);
        auto pel = std::make_unique<PEL>(data);
        repo.add(pel);

        // BMC info
        data = pelFactory(id++, 'O', 0x0, 0x8800, 500);
        pel = std::make_unique<PEL>(data);
        repo.add(pel);

        // Hostboot predictive
        data = pelFactory(id++, 'B', 0x20, 0x8800, 500);
        pel = std::make_unique<PEL>(data);
        repo.add(pel);

        // Hostboot info
        data = pelFactory(id++, 'B', 0x0, 0x8800, 500);
        pel = std::make_unique<PEL>(data);
        repo.add(pel);
    }

    const auto& sizes = repo.getSizeStats();
    EXPECT_EQ(sizes.total, 20000);

    // Prune down to 15%/30%/15%/30% = 90% total
    auto IDs = repo.prune();

    // Check the final sizes
    EXPECT_EQ(sizes.total, 9000);
    EXPECT_EQ(sizes.bmcInfo, 1500);           // 15% of 10000
    EXPECT_EQ(sizes.bmcServiceable, 3000);    // 30% of 10000
    EXPECT_EQ(sizes.nonBMCInfo, 1500);        // 15% of 10000
    EXPECT_EQ(sizes.nonBMCServiceable, 3000); // 30% of 10000
}

// Test that if filled completely with 1 type of PEL, that
// pruning still works properly
TEST_F(RepositoryTest, TestPruneInfoOnly)
{
    uint32_t id = 1;
    Repository repo{repoPath, 10000};

    // Fill 10000B of BMC info PELs
    for (int i = 0; i < 10; i++)
    {
        // BMC predictive
        auto data = pelFactory(id++, 'O', 0, 0x8800, 1000);
        auto pel = std::make_unique<PEL>(data);
        repo.add(pel);
    }

    const auto& sizes = repo.getSizeStats();
    EXPECT_EQ(sizes.total, 10000);

    auto IDs = repo.prune();

    // Check the final sizes
    EXPECT_EQ(sizes.total, 1000);
    EXPECT_EQ(sizes.bmcInfo, 1000); // < 15% of 10000
    EXPECT_EQ(sizes.bmcServiceable, 0);
    EXPECT_EQ(sizes.nonBMCInfo, 0);
    EXPECT_EQ(sizes.nonBMCServiceable, 0);

    EXPECT_EQ(IDs.size(), 9);
}

// Test that the HMC/OS/PHYP ack values affect the
// pruning order.
TEST_F(RepositoryTest, TestPruneWithAcks)
{
    uint32_t id = 1;
    Repository repo{repoPath, 10000};

    // Fill 30% worth of BMC non-info non-acked PELs
    for (int i = 0; i < 6; i++)
    {
        // BMC predictive
        auto data = pelFactory(id++, 'O', 0x20, 0x8800, 500);
        auto pel = std::make_unique<PEL>(data);
        repo.add(pel);
    }

    // Add another PEL to push it over the 30%, each time adding
    // a different type that should be pruned before the above ones
    // even though those are older.
    for (int i = 0; i < 3; i++)
    {
        auto data = pelFactory(id++, 'O', 0x20, 0x8800, 500);
        auto pel = std::make_unique<PEL>(data);
        auto idToDelete = pel->obmcLogID();
        repo.add(pel);

        if (0 == i)
        {
            repo.setPELHMCTransState(pel->id(), TransmissionState::acked);
        }
        else if (1 == i)
        {
            repo.setPELHostTransState(pel->id(), TransmissionState::acked);
        }
        else
        {
            repo.setPELHostTransState(pel->id(), TransmissionState::sent);
        }

        auto IDs = repo.prune();
        EXPECT_EQ(repo.getSizeStats().total, 3000);

        // The newest PEL should be the one deleted
        ASSERT_EQ(IDs.size(), 1);
        EXPECT_EQ(IDs[0], idToDelete);
    }
}
