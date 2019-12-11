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

// Test that PELs are enqueued ons startup
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

    sd_event* event = nullptr;
    auto r = sd_event_default(&event);
    ASSERT_TRUE(r >= 0);

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
