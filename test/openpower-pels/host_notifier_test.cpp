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
#include "extensions/openpower-pels/data_interface.hpp"
#include "extensions/openpower-pels/host_notifier.hpp"
#include "mocks.hpp"
#include "pel_utils.hpp"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <chrono>

#include <gtest/gtest.h>

using namespace openpower::pels;
using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;
namespace fs = std::filesystem;
using namespace std::chrono;

const size_t actionFlags0Offset = 66;
const size_t actionFlags1Offset = 67;

class HostNotifierTest : public CleanPELFiles
{
  public:
    HostNotifierTest()
    {
        auto r = sd_event_default(&event);
        EXPECT_TRUE(r >= 0);
    }

    ~HostNotifierTest()
    {
        sd_event_unref(event);
    }

  protected:
    sd_event* event;
};

/**
 * @brief Create PEL with the specified action flags
 *
 * @param[in] actionFlagsMask - Optional action flags to use
 *
 * @return std::unique_ptr<PEL>
 */
std::unique_ptr<PEL> makePEL(uint16_t actionFlagsMask = 0)
{
    static uint32_t obmcID = 1;
    auto data = pelDataFactory(TestPELType::pelSimple);

    data[actionFlags0Offset] |= actionFlagsMask >> 8;
    data[actionFlags1Offset] |= actionFlagsMask & 0xFF;

    auto pel = std::make_unique<PEL>(data, obmcID++);
    pel->assignID();
    pel->setCommitTime();
    return pel;
}

/**
 * @brief Run an iteration of the event loop.
 *
 * An event loop is used for:
 *   1) timer expiration callbacks
 *   2) Dispatches
 *   3) host interface receive callbacks
 *
 * @param[in] event - The event object
 * @param[in] numEvents - number of times to call Event::run()
 * @param[in] timeout - timeout value for run()
 */
void runEvents(sdeventplus::Event& event, size_t numEvents,
               milliseconds timeout = milliseconds(1))
{
    for (size_t i = 0; i < numEvents; i++)
    {
        event.run(timeout);
    }
}

// Test that host state change callbacks work
TEST_F(HostNotifierTest, TestHostStateChange)
{
    MockDataInterface dataIface;

    bool hostState = false;
    bool called = false;
    DataInterfaceBase::HostStateChangeFunc func = [&hostState,
                                                   &called](bool state) {
        hostState = state;
        called = true;
    };

    dataIface.subscribeToHostStateChange("test", func);

    // callback called
    dataIface.changeHostState(true);
    EXPECT_TRUE(called);
    EXPECT_TRUE(hostState);

    // No change, not called
    called = false;
    dataIface.changeHostState(true);
    EXPECT_FALSE(called);

    // Called again
    dataIface.changeHostState(false);
    EXPECT_FALSE(hostState);
    EXPECT_TRUE(called);

    // Shouldn't get called after an unsubscribe
    dataIface.unsubscribeFromHostStateChange("test");

    called = false;

    dataIface.changeHostState(true);
    EXPECT_FALSE(called);
}

// Test dealing with how acked PELs are put on the
// notification queue.
TEST_F(HostNotifierTest, TestPolicyAckedPEL)
{
    Repository repo{repoPath};
    MockDataInterface dataIface;

    std::unique_ptr<HostInterface> hostIface =
        std::make_unique<MockHostInterface>(event, dataIface);

    HostNotifier notifier{repo, dataIface, std::move(hostIface)};

    auto pel = makePEL();
    repo.add(pel);

    // This is required
    EXPECT_TRUE(notifier.enqueueRequired(pel->id()));
    EXPECT_TRUE(notifier.notifyRequired(pel->id()));

    // Not in the repo
    EXPECT_FALSE(notifier.enqueueRequired(42));
    EXPECT_FALSE(notifier.notifyRequired(42));

    // Now set this PEL to host acked
    repo.setPELHostTransState(pel->id(), TransmissionState::acked);

    // Since it's acked, doesn't need to be enqueued or transmitted
    EXPECT_FALSE(notifier.enqueueRequired(pel->id()));
    EXPECT_FALSE(notifier.notifyRequired(pel->id()));
}

// Test the 'don't report' PEL flag
TEST_F(HostNotifierTest, TestPolicyDontReport)
{
    Repository repo{repoPath};
    MockDataInterface dataIface;

    std::unique_ptr<HostInterface> hostIface =
        std::make_unique<MockHostInterface>(event, dataIface);

    HostNotifier notifier{repo, dataIface, std::move(hostIface)};

    // dontReportToHostFlagBit
    auto pel = makePEL(0x1000);

    // Double check the action flag is still set
    std::bitset<16> actionFlags = pel->userHeader().actionFlags();
    EXPECT_TRUE(actionFlags.test(dontReportToHostFlagBit));

    repo.add(pel);

    // Don't need to send this to the host
    EXPECT_FALSE(notifier.enqueueRequired(pel->id()));
}

// Test that hidden PELs need notification when there
// is no HMC.
TEST_F(HostNotifierTest, TestPolicyHiddenNoHMC)
{
    Repository repo{repoPath};
    MockDataInterface dataIface;

    std::unique_ptr<HostInterface> hostIface =
        std::make_unique<MockHostInterface>(event, dataIface);

    HostNotifier notifier{repo, dataIface, std::move(hostIface)};

    // hiddenFlagBit
    auto pel = makePEL(0x4000);

    // Double check the action flag is still set
    std::bitset<16> actionFlags = pel->userHeader().actionFlags();
    EXPECT_TRUE(actionFlags.test(hiddenFlagBit));

    repo.add(pel);

    // Still need to enqueue this
    EXPECT_TRUE(notifier.enqueueRequired(pel->id()));

    // Still need to send it
    EXPECT_TRUE(notifier.notifyRequired(pel->id()));
}

// Don't need to enqueue a hidden log already acked by the HMC
TEST_F(HostNotifierTest, TestPolicyHiddenWithHMCAcked)
{
    Repository repo{repoPath};
    MockDataInterface dataIface;

    std::unique_ptr<HostInterface> hostIface =
        std::make_unique<MockHostInterface>(event, dataIface);

    HostNotifier notifier{repo, dataIface, std::move(hostIface)};

    // hiddenFlagBit
    auto pel = makePEL(0x4000);

    // Double check the action flag is still set
    std::bitset<16> actionFlags = pel->userHeader().actionFlags();
    EXPECT_TRUE(actionFlags.test(hiddenFlagBit));

    repo.add(pel);

    // No HMC yet, so required
    EXPECT_TRUE(notifier.enqueueRequired(pel->id()));

    repo.setPELHMCTransState(pel->id(), TransmissionState::acked);

    // Not required anymore
    EXPECT_FALSE(notifier.enqueueRequired(pel->id()));
}

// Test that changing the HMC manage status affects
// the policy with hidden log notification.
TEST_F(HostNotifierTest, TestPolicyHiddenWithHMCManaged)
{
    Repository repo{repoPath};
    MockDataInterface dataIface;

    std::unique_ptr<HostInterface> hostIface =
        std::make_unique<MockHostInterface>(event, dataIface);

    HostNotifier notifier{repo, dataIface, std::move(hostIface)};

    // hiddenFlagBit
    auto pel = makePEL(0x4000);

    repo.add(pel);

    // The first time, the HMC managed is false
    EXPECT_TRUE(notifier.notifyRequired(pel->id()));

    dataIface.setHMCManaged(true);

    // This time, HMC managed is true so no need to notify
    EXPECT_FALSE(notifier.notifyRequired(pel->id()));
}

// Test that PELs are enqueued on startup
TEST_F(HostNotifierTest, TestStartup)
{
    Repository repo{repoPath};
    MockDataInterface dataIface;

    // Give the repo 10 PELs to start with
    for (int i = 0; i < 10; i++)
    {
        auto pel = makePEL();
        repo.add(pel);
    }

    std::unique_ptr<HostInterface> hostIface =
        std::make_unique<MockHostInterface>(event, dataIface);

    HostNotifier notifier{repo, dataIface, std::move(hostIface)};

    ASSERT_EQ(notifier.queueSize(), 10);

    // Now add 10 more after the notifier is watching
    for (int i = 0; i < 10; i++)
    {
        auto pel = makePEL();
        repo.add(pel);
    }

    ASSERT_EQ(notifier.queueSize(), 20);
}

// Test the simple path were PELs get sent to the host
TEST_F(HostNotifierTest, TestSendCmd)
{
    Repository repo{repoPath};
    MockDataInterface dataIface;

    sdeventplus::Event sdEvent{event};

    std::unique_ptr<HostInterface> hostIface =
        std::make_unique<MockHostInterface>(event, dataIface);

    MockHostInterface& mockHostIface =
        reinterpret_cast<MockHostInterface&>(*hostIface);

    HostNotifier notifier{repo, dataIface, std::move(hostIface)};

    auto send = [&mockHostIface](uint32_t id, uint32_t size) {
        return mockHostIface.send(0);
    };

    EXPECT_CALL(mockHostIface, sendNewLogCmd(_, _))
        .WillRepeatedly(Invoke(send));

    // Add a PEL with the host off
    auto pel = makePEL();
    repo.add(pel);

    EXPECT_EQ(notifier.queueSize(), 1);

    dataIface.changeHostState(true);

    runEvents(sdEvent, 1);

    // It was sent up
    EXPECT_EQ(mockHostIface.numCmdsProcessed(), 1);
    EXPECT_EQ(notifier.queueSize(), 0);

    // Verify the state was written to the PEL.
    Repository::LogID id{Repository::LogID::Pel{pel->id()}};
    auto data = repo.getPELData(id);
    PEL pelFromRepo{*data};
    EXPECT_EQ(pelFromRepo.hostTransmissionState(), TransmissionState::sent);

    // Add a few more PELs.  They will get sent.
    pel = makePEL();
    repo.add(pel);

    // Dispatch it by hitting the event loop (no commands sent yet)
    // Don't need to test this step discretely in the future
    runEvents(sdEvent, 1);
    EXPECT_EQ(mockHostIface.numCmdsProcessed(), 1);
    EXPECT_EQ(notifier.queueSize(), 0);

    // Send the command
    runEvents(sdEvent, 1);

    EXPECT_EQ(mockHostIface.numCmdsProcessed(), 2);
    EXPECT_EQ(notifier.queueSize(), 0);

    pel = makePEL();
    repo.add(pel);

    // dispatch and process the command
    runEvents(sdEvent, 2);

    EXPECT_EQ(mockHostIface.numCmdsProcessed(), 3);
    EXPECT_EQ(notifier.queueSize(), 0);
}

// Test that if the class is created with the host up,
// it will send PELs
TEST_F(HostNotifierTest, TestStartAfterHostUp)
{
    Repository repo{repoPath};
    MockDataInterface dataIface;

    // Add PELs right away
    auto pel = makePEL();
    repo.add(pel);
    pel = makePEL();
    repo.add(pel);

    sdeventplus::Event sdEvent{event};

    std::unique_ptr<HostInterface> hostIface =
        std::make_unique<MockHostInterface>(event, dataIface);

    MockHostInterface& mockHostIface =
        reinterpret_cast<MockHostInterface&>(*hostIface);

    auto send = [&mockHostIface](uint32_t id, uint32_t size) {
        return mockHostIface.send(0);
    };

    EXPECT_CALL(mockHostIface, sendNewLogCmd(_, _))
        .WillRepeatedly(Invoke(send));

    // Create the HostNotifier class with the host already up
    dataIface.changeHostState(true);
    HostNotifier notifier{repo, dataIface, std::move(hostIface)};

    // It should start sending PELs right away
    runEvents(sdEvent, 2);

    EXPECT_EQ(mockHostIface.numCmdsProcessed(), 2);
    EXPECT_EQ(notifier.queueSize(), 0);
}

// Test that a single failure will cause a retry
TEST_F(HostNotifierTest, TestHostRetry)
{
    Repository repo{repoPath};
    MockDataInterface dataIface;

    sdeventplus::Event sdEvent{event};

    std::unique_ptr<HostInterface> hostIface =
        std::make_unique<MockHostInterface>(event, dataIface);

    MockHostInterface& mockHostIface =
        reinterpret_cast<MockHostInterface&>(*hostIface);

    HostNotifier notifier{repo, dataIface, std::move(hostIface)};

    auto sendFailure = [&mockHostIface](uint32_t id, uint32_t size) {
        return mockHostIface.send(1);
    };
    auto sendSuccess = [&mockHostIface](uint32_t id, uint32_t size) {
        return mockHostIface.send(0);
    };

    EXPECT_CALL(mockHostIface, sendNewLogCmd(_, _))
        .WillOnce(Invoke(sendFailure))
        .WillOnce(Invoke(sendSuccess))
        .WillOnce(Invoke(sendSuccess));

    dataIface.changeHostState(true);

    auto pel = makePEL();
    repo.add(pel);

    // Dispatch and handle the command
    runEvents(sdEvent, 2);

    // The command failed, so the queue isn't empty
    EXPECT_EQ(mockHostIface.numCmdsProcessed(), 1);
    EXPECT_EQ(notifier.queueSize(), 1);

    // Run the events again to let the timer expire and the
    // command to be retried, which will be successful.
    runEvents(sdEvent, 2, mockHostIface.getReceiveRetryDelay());

    EXPECT_EQ(mockHostIface.numCmdsProcessed(), 2);
    EXPECT_EQ(notifier.queueSize(), 0);

    // This one should pass with no problems
    pel = makePEL();
    repo.add(pel);

    // Dispatch and handle the command
    runEvents(sdEvent, 2);

    EXPECT_EQ(mockHostIface.numCmdsProcessed(), 3);
    EXPECT_EQ(notifier.queueSize(), 0);
}

// Test that all commands fail and notifier will give up
TEST_F(HostNotifierTest, TestHardFailure)
{
    Repository repo{repoPath};
    MockDataInterface dataIface;

    sdeventplus::Event sdEvent{event};

    std::unique_ptr<HostInterface> hostIface =
        std::make_unique<MockHostInterface>(event, dataIface);

    MockHostInterface& mockHostIface =
        reinterpret_cast<MockHostInterface&>(*hostIface);

    HostNotifier notifier{repo, dataIface, std::move(hostIface)};

    // Every call will fail
    auto sendFailure = [&mockHostIface](uint32_t id, uint32_t size) {
        return mockHostIface.send(1);
    };

    EXPECT_CALL(mockHostIface, sendNewLogCmd(_, _))
        .WillRepeatedly(Invoke(sendFailure));

    dataIface.changeHostState(true);

    auto pel = makePEL();
    repo.add(pel);

    // Clock more retries than necessary
    runEvents(sdEvent, 40, mockHostIface.getReceiveRetryDelay());

    // Should have stopped after the 15 Tries
    EXPECT_EQ(mockHostIface.numCmdsProcessed(), 15);
    EXPECT_EQ(notifier.queueSize(), 1);

    // Now add another PEL, and it should start trying again
    // though it will also eventually give up
    pel = makePEL();
    repo.add(pel);

    runEvents(sdEvent, 40, mockHostIface.getReceiveRetryDelay());

    // Tried an additional 15 times
    EXPECT_EQ(mockHostIface.numCmdsProcessed(), 30);
    EXPECT_EQ(notifier.queueSize(), 2);
}

// Cancel an in progress command
TEST_F(HostNotifierTest, TestCancelCmd)
{
    Repository repo{repoPath};
    MockDataInterface dataIface;

    sdeventplus::Event sdEvent{event};

    std::unique_ptr<HostInterface> hostIface =
        std::make_unique<MockHostInterface>(event, dataIface);

    MockHostInterface& mockHostIface =
        reinterpret_cast<MockHostInterface&>(*hostIface);

    HostNotifier notifier{repo, dataIface, std::move(hostIface)};

    auto send = [&mockHostIface](uint32_t id, uint32_t size) {
        return mockHostIface.send(0);
    };

    EXPECT_CALL(mockHostIface, sendNewLogCmd(_, _))
        .WillRepeatedly(Invoke(send));

    dataIface.changeHostState(true);

    // Add and send one PEL, but don't enter the event loop
    // so the receive function can't run.
    auto pel = makePEL();
    repo.add(pel);

    // Not dispatched yet
    EXPECT_EQ(notifier.queueSize(), 1);

    // Dispatch it
    runEvents(sdEvent, 1);

    // It was sent and off the queue
    EXPECT_EQ(notifier.queueSize(), 0);

    // This will cancel the receive
    dataIface.changeHostState(false);

    // Back on the queue
    EXPECT_EQ(notifier.queueSize(), 1);

    // Turn the host back on and make sure
    // commands will work again
    dataIface.changeHostState(true);

    runEvents(sdEvent, 1);

    EXPECT_EQ(mockHostIface.numCmdsProcessed(), 1);
    EXPECT_EQ(notifier.queueSize(), 0);
}

// Test that acking a PEL persist across power cycles
TEST_F(HostNotifierTest, TestPowerCycleAndAcks)
{
    Repository repo{repoPath};
    MockDataInterface dataIface;

    sdeventplus::Event sdEvent{event};

    std::unique_ptr<HostInterface> hostIface =
        std::make_unique<MockHostInterface>(event, dataIface);

    MockHostInterface& mockHostIface =
        reinterpret_cast<MockHostInterface&>(*hostIface);

    HostNotifier notifier{repo, dataIface, std::move(hostIface)};

    auto send = [&mockHostIface](uint32_t id, uint32_t size) {
        return mockHostIface.send(0);
    };

    EXPECT_CALL(mockHostIface, sendNewLogCmd(_, _))
        .WillRepeatedly(Invoke(send));

    // Add 2 PELs with host off
    auto pel = makePEL();
    repo.add(pel);
    auto id1 = pel->id();

    pel = makePEL();
    repo.add(pel);
    auto id2 = pel->id();

    dataIface.changeHostState(true);

    runEvents(sdEvent, 2);

    // The were both sent.
    EXPECT_EQ(mockHostIface.numCmdsProcessed(), 2);
    EXPECT_EQ(notifier.queueSize(), 0);

    dataIface.changeHostState(false);

    // Those PELs weren't acked, so they will get sent again
    EXPECT_EQ(notifier.queueSize(), 2);

    // Power back on and send them again
    dataIface.changeHostState(true);
    runEvents(sdEvent, 2);

    EXPECT_EQ(mockHostIface.numCmdsProcessed(), 4);
    EXPECT_EQ(notifier.queueSize(), 0);

    // Ack them and verify the state in the PEL.
    notifier.ackPEL(id1);
    notifier.ackPEL(id2);

    Repository::LogID id{Repository::LogID::Pel{id1}};
    auto data = repo.getPELData(id);
    PEL pelFromRepo1{*data};
    EXPECT_EQ(pelFromRepo1.hostTransmissionState(), TransmissionState::acked);

    id.pelID.id = id2;
    data = repo.getPELData(id);
    PEL pelFromRepo2{*data};
    EXPECT_EQ(pelFromRepo2.hostTransmissionState(), TransmissionState::acked);

    // Power back off, and they should't get re-added
    dataIface.changeHostState(false);

    EXPECT_EQ(notifier.queueSize(), 0);
}

// Test the host full condition
TEST_F(HostNotifierTest, TestHostFull)
{
    // The full interaction with the host is:
    // BMC:  new PEL available
    // Host: ReadPELFile  (not modeled here)
    // Host: Ack(id) (if not full), or HostFull(id)
    // BMC: if full and any new PELs come in, don't sent them
    // Start a timer and try again
    // Host responds with either Ack or full
    // and repeat

    Repository repo{repoPath};
    MockDataInterface dataIface;

    sdeventplus::Event sdEvent{event};

    std::unique_ptr<HostInterface> hostIface =
        std::make_unique<MockHostInterface>(event, dataIface);

    MockHostInterface& mockHostIface =
        reinterpret_cast<MockHostInterface&>(*hostIface);

    HostNotifier notifier{repo, dataIface, std::move(hostIface)};

    auto send = [&mockHostIface](uint32_t id, uint32_t size) {
        return mockHostIface.send(0);
    };

    EXPECT_CALL(mockHostIface, sendNewLogCmd(_, _))
        .WillRepeatedly(Invoke(send));

    dataIface.changeHostState(true);

    // Add and dispatch/send one PEL
    auto pel = makePEL();
    auto id = pel->id();
    repo.add(pel);
    runEvents(sdEvent, 2);

    EXPECT_EQ(mockHostIface.numCmdsProcessed(), 1);
    EXPECT_EQ(notifier.queueSize(), 0);

    // Host is full
    notifier.setHostFull(id);

    // It goes back on the queue
    EXPECT_EQ(notifier.queueSize(), 1);

    // The transmission state goes back to new
    Repository::LogID i{Repository::LogID::Pel{id}};
    auto data = repo.getPELData(i);
    PEL pelFromRepo{*data};
    EXPECT_EQ(pelFromRepo.hostTransmissionState(), TransmissionState::newPEL);

    // Clock it, nothing should be sent still.
    runEvents(sdEvent, 1);

    EXPECT_EQ(mockHostIface.numCmdsProcessed(), 1);
    EXPECT_EQ(notifier.queueSize(), 1);

    // Add another PEL and clock it, still nothing sent
    pel = makePEL();
    repo.add(pel);
    runEvents(sdEvent, 2);
    EXPECT_EQ(mockHostIface.numCmdsProcessed(), 1);
    EXPECT_EQ(notifier.queueSize(), 2);

    // Let the host full timer expire to trigger a retry.
    // Add some extra event passes just to be sure nothing new is sent.
    runEvents(sdEvent, 5, mockHostIface.getHostFullRetryDelay());

    // The timer expiration will send just the 1, not both
    EXPECT_EQ(mockHostIface.numCmdsProcessed(), 2);
    EXPECT_EQ(notifier.queueSize(), 1);

    // Host still full
    notifier.setHostFull(id);

    // Let the host full timer attempt again
    runEvents(sdEvent, 2, mockHostIface.getHostFullRetryDelay());
    EXPECT_EQ(mockHostIface.numCmdsProcessed(), 3);

    // Add yet another PEL with the retry timer expired.
    // It shouldn't get sent out.
    pel = makePEL();
    repo.add(pel);
    runEvents(sdEvent, 2);
    EXPECT_EQ(mockHostIface.numCmdsProcessed(), 3);

    // The last 2 PELs still on the queue
    EXPECT_EQ(notifier.queueSize(), 2);

    // Host no longer full, it finally acks the first PEL
    notifier.ackPEL(id);

    // Now the remaining 2 PELs will be dispatched
    runEvents(sdEvent, 3);

    EXPECT_EQ(mockHostIface.numCmdsProcessed(), 5);
    EXPECT_EQ(notifier.queueSize(), 0);
}
