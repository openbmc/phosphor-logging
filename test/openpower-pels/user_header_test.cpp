// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "elog_entry.hpp"
#include "extensions/openpower-pels/pel_types.hpp"
#include "extensions/openpower-pels/private_header.hpp"
#include "extensions/openpower-pels/user_header.hpp"
#include "mocks.hpp"
#include "pel_utils.hpp"

#include <gtest/gtest.h>

using namespace openpower::pels;
using ::testing::Return;

TEST(UserHeaderTest, SizeTest)
{
    EXPECT_EQ(UserHeader::flattenedSize(), 24);
}

TEST(UserHeaderTest, UnflattenFlattenTest)
{
    auto data = pelDataFactory(TestPELType::userHeaderSection);

    Stream stream(data);
    UserHeader uh(stream);
    EXPECT_EQ(uh.valid(), true);

    EXPECT_EQ(uh.header().id, 0x5548);
    EXPECT_EQ(uh.header().size, UserHeader::flattenedSize());
    EXPECT_EQ(uh.header().version, 0x01);
    EXPECT_EQ(uh.header().subType, 0x0A);
    EXPECT_EQ(uh.header().componentID, 0x0B0C);

    EXPECT_EQ(uh.subsystem(), 0x10);
    EXPECT_EQ(uh.scope(), 0x04);
    EXPECT_EQ(uh.severity(), 0x20);
    EXPECT_EQ(uh.eventType(), 0x00);
    EXPECT_EQ(uh.problemDomain(), 0x03);
    EXPECT_EQ(uh.problemVector(), 0x04);
    EXPECT_EQ(uh.actionFlags(), 0x80C0);

    // Now flatten into a vector and check that this vector
    // matches the original one.
    std::vector<uint8_t> newData;
    Stream newStream(newData);

    uh.flatten(newStream);
    EXPECT_EQ(data, newData);
}

TEST(UserHeaderTest, ShortDataTest)
{
    auto data = pelDataFactory(TestPELType::userHeaderSection);
    data.resize(data.size() - 1);

    Stream stream(data);
    UserHeader uh(stream);

    EXPECT_EQ(uh.valid(), false);
}

TEST(UserHeaderTest, CorruptDataTest1)
{
    auto data = pelDataFactory(TestPELType::userHeaderSection);
    data.resize(data.size() - 1);

    data.at(0) = 0; // corrupt the section ID

    Stream stream(data);
    UserHeader uh(stream);

    EXPECT_EQ(uh.valid(), false);
}

TEST(UserHeaderTest, CorruptDataTest2)
{
    auto data = pelDataFactory(TestPELType::userHeaderSection);

    data.at(4) = 0x22; // corrupt the version

    Stream stream(data);
    UserHeader uh(stream);

    EXPECT_EQ(uh.valid(), false);
}

// Construct the User Header from the message registry
TEST(UserHeaderTest, ConstructionTest)
{
    using namespace openpower::pels::message;
    {
        Entry regEntry;

        regEntry.name = "test";
        regEntry.subsystem = 5;
        regEntry.severity = {{"", 0x40}};
        regEntry.actionFlags = 0xC000;
        regEntry.eventType = 1;
        regEntry.eventScope = 2;

        MockDataInterface dataIface;
        AdditionalData ad;

        UserHeader uh(regEntry, phosphor::logging::Entry::Level::Error, ad,
                      dataIface);

        ASSERT_TRUE(uh.valid());
        EXPECT_EQ(uh.header().id, 0x5548);
        EXPECT_EQ(uh.header().size, UserHeader::flattenedSize());
        EXPECT_EQ(uh.header().version, 0x01);
        EXPECT_EQ(uh.header().subType, 0x00);
        EXPECT_EQ(uh.header().componentID,
                  static_cast<uint16_t>(ComponentID::phosphorLogging));

        EXPECT_EQ(uh.subsystem(), 5);
        EXPECT_EQ(uh.severity(), 0x40);
        EXPECT_EQ(uh.eventType(), 1);
        EXPECT_EQ(uh.scope(), 2);
        EXPECT_EQ(uh.problemDomain(), 0);
        EXPECT_EQ(uh.problemVector(), 0);
        EXPECT_EQ(uh.actionFlags(), 0xC000);

        {
            // The same thing, but as if the action flags weren't specified
            // in the registry so they are a nullopt.  The object should
            // then set them to 0xFFFF.
            regEntry.actionFlags = std::nullopt;

            UserHeader uh(regEntry, phosphor::logging::Entry::Level::Error, ad,
                          dataIface);
            EXPECT_EQ(uh.actionFlags(), 0xFFFF);
        }
    }

    // Test the system type based severity lookups
    {
        Entry regEntry;

        regEntry.name = "test";
        regEntry.subsystem = 5;
        regEntry.severity = {{"", 0x20}, {"systemB", 0x10}, {"systemA", 0x00}};

        AdditionalData ad;

        MockDataInterface dataIface;
        std::vector<std::string> names1{"systemA"};
        std::vector<std::string> names2{"systemB"};
        std::vector<std::string> names3{"systemC"};

        EXPECT_CALL(dataIface, getSystemNames)
            .WillOnce(Return(names1))
            .WillOnce(Return(names2))
            .WillOnce(Return(names3));

        {
            UserHeader uh(regEntry, phosphor::logging::Entry::Level::Error, ad,
                          dataIface);

            EXPECT_EQ(uh.severity(), 0x00);
        }

        {
            UserHeader uh(regEntry, phosphor::logging::Entry::Level::Error, ad,
                          dataIface);

            EXPECT_EQ(uh.severity(), 0x10);
        }

        {
            UserHeader uh(regEntry, phosphor::logging::Entry::Level::Error, ad,
                          dataIface);

            EXPECT_EQ(uh.severity(), 0x20);
        }
    }
}

// Test that the severity comes from the event log if not
// in the message registry
TEST(UserHeaderTest, UseEventLogSevTest)
{
    using namespace openpower::pels::message;
    Entry regEntry;

    regEntry.name = "test";
    regEntry.subsystem = 5;
    regEntry.actionFlags = 0xC000;
    regEntry.eventType = 1;
    regEntry.eventScope = 2;
    // Leave off severity

    MockDataInterface dataIface;
    AdditionalData ad;

    UserHeader uh(regEntry, phosphor::logging::Entry::Level::Error, ad,
                  dataIface);
    ASSERT_EQ(uh.severity(), 0x40);
}

// Test that the critical severity comes from the event log if not
// in the message registry
TEST(UserHeaderTest, UseEventLogSevCritTest)
{
    using namespace openpower::pels::message;
    Entry regEntry;

    regEntry.name = "test";
    regEntry.subsystem = 5;
    regEntry.actionFlags = 0xC000;
    regEntry.eventType = 1;
    regEntry.eventScope = 2;
    // Leave off severity

    MockDataInterface dataIface;
    AdditionalData ad;

    UserHeader uh(regEntry, phosphor::logging::Entry::Level::Critical, ad,
                  dataIface);
    ASSERT_EQ(uh.severity(), 0x50);
}

// Test that the critical severity comes from the event log if not
// in the message registry and termination condition is set
TEST(UserHeaderTest, UseEventLogSevCritTermTest)
{
    using namespace openpower::pels::message;
    Entry regEntry;

    regEntry.name = "test";
    regEntry.subsystem = 5;
    regEntry.actionFlags = 0xC000;
    regEntry.eventType = 1;
    regEntry.eventScope = 2;
    // Leave off severity

    MockDataInterface dataIface;
    std::map<std::string, std::string> adData{
        {"SEVERITY_DETAIL", "SYSTEM_TERM"}};
    AdditionalData ad{adData};

    UserHeader uh(regEntry, phosphor::logging::Entry::Level::Critical, ad,
                  dataIface);
    ASSERT_EQ(uh.severity(), 0x51);
}

// Test that the optional event type & scope fields work
TEST(UserHeaderTest, DefaultEventTypeScopeTest)
{
    using namespace openpower::pels::message;
    Entry regEntry;

    regEntry.name = "test";
    regEntry.subsystem = 5;
    regEntry.severity = {{"", 0x40}};
    regEntry.actionFlags = 0xC000;

    MockDataInterface dataIface;
    AdditionalData ad;

    UserHeader uh(regEntry, phosphor::logging::Entry::Level::Error, ad,
                  dataIface);

    ASSERT_EQ(uh.eventType(), 0);
    ASSERT_EQ(uh.scope(), 0x03);
}

// Test that the event severity & action flags override
// when QuiesceOnHwError is set
TEST(UserHeaderTest, UseEventLogQuiesceOnErrorTest)
{
    using namespace openpower::pels::message;
    Entry regEntry;

    regEntry.name = "test";
    regEntry.subsystem = 5;
    regEntry.actionFlags = 0xC000;
    regEntry.eventType = 1;
    regEntry.eventScope = 2;
    regEntry.severity = {{"", 0x40}, {"systemB", 0x10}, {"systemA", 0x00}};

    // set the value for mfg severity and action flags
    regEntry.mfgSeverity = {{"systemA", 0x20}};
    regEntry.mfgActionFlags = 0xF000;

    std::vector<std::string> names{"systemA"};

    MockDataInterface dataIface;
    AdditionalData ad;

    EXPECT_CALL(dataIface, getSystemNames).WillOnce(Return(names));
    EXPECT_CALL(dataIface, getQuiesceOnError).WillOnce(Return(true));

    UserHeader uh(regEntry, phosphor::logging::Entry::Level::Error, ad,
                  dataIface);

    EXPECT_EQ(uh.severity(), 0x20);
    EXPECT_EQ(uh.actionFlags(), 0xF000);
}

// Test that the PEL Subsystem omes from the event log if any
TEST(UserHeaderTest, UseEventLogPELSubsystem)
{
    using namespace openpower::pels::message;

    {
        Entry regEntry;

        regEntry.name = "test";
        regEntry.subsystem = 5;
        regEntry.actionFlags = 0xC000;
        regEntry.eventType = 1;
        regEntry.eventScope = 2;

        MockDataInterface dataIface;
        std::map<std::string, std::string> adData{{"PEL_SUBSYSTEM", "0x25"}};
        AdditionalData ad{adData};

        UserHeader uh(regEntry, phosphor::logging::Entry::Level::Critical, ad,
                      dataIface);
        ASSERT_EQ(uh.subsystem(), 0x25);
    }
    {
        // No subsystem in registry, and invalid PEL_SUBSYSTEM
        Entry regEntry;

        regEntry.name = "test";
        regEntry.actionFlags = 0xC000;
        regEntry.eventType = 1;
        regEntry.eventScope = 2;

        MockDataInterface dataIface;
        std::map<std::string, std::string> adData{{"PEL_SUBSYSTEM", "0x99"}};
        AdditionalData ad{adData};

        UserHeader uh(regEntry, phosphor::logging::Entry::Level::Critical, ad,
                      dataIface);
        ASSERT_EQ(uh.subsystem(), 0x70); // others
    }
    {
        // No subsystem in registry or PEL_SUBSYSTEM
        Entry regEntry;

        regEntry.name = "test";
        regEntry.actionFlags = 0xC000;
        regEntry.eventType = 1;
        regEntry.eventScope = 2;

        MockDataInterface dataIface;
        std::map<std::string, std::string> adData;
        AdditionalData ad{adData};

        UserHeader uh(regEntry, phosphor::logging::Entry::Level::Critical, ad,
                      dataIface);
        ASSERT_EQ(uh.subsystem(), 0x70); // others
    }
}
