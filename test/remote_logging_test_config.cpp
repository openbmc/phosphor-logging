#include "remote_logging_tests.hpp"

#include <fstream>
#include <string>

namespace phosphor
{

namespace rsyslog_config::internal
{
extern std::optional<
    std::tuple<std::string, uint32_t, NetworkClient::TransportProtocol>>
    parseConfig(std::istream& ss);
}

namespace logging
{
namespace test
{

std::string getConfig(const char* filePath)
{
    std::fstream stream(filePath, std::fstream::in);
    std::string line;
    std::getline(stream, line);
    return line;
}

TEST_F(TestRemoteLogging, testOnlyAddress)
{
    config->address("1.1.1.1");
    EXPECT_EQ(getConfig(configFilePath.c_str()), "*.* /dev/null");
}

TEST_F(TestRemoteLogging, testOnlyPort)
{
    config->port(100);
    EXPECT_EQ(getConfig(configFilePath.c_str()), "*.* /dev/null");
}

TEST_F(TestRemoteLogging, testGoodConfig)
{
    config->address("1.1.1.1");
    config->port(100);
    EXPECT_EQ(getConfig(configFilePath.c_str()), "*.* @@1.1.1.1:100");
}

TEST_F(TestRemoteLogging, testClearAddress)
{
    config->address("1.1.1.1");
    config->port(100);
    EXPECT_EQ(getConfig(configFilePath.c_str()), "*.* @@1.1.1.1:100");

    config->address("");
    EXPECT_EQ(getConfig(configFilePath.c_str()), "*.* /dev/null");
}

TEST_F(TestRemoteLogging, testClearPort)
{
    config->address("1.1.1.1");
    config->port(100);
    EXPECT_EQ(getConfig(configFilePath.c_str()), "*.* @@1.1.1.1:100");

    config->port(0);
    EXPECT_EQ(getConfig(configFilePath.c_str()), "*.* /dev/null");
}

TEST_F(TestRemoteLogging, testGoodIPv6Config)
{
    config->address("abcd:ef01::01");
    config->port(50000);
    EXPECT_EQ(getConfig(configFilePath.c_str()),
              "*.* @@[abcd:ef01::01]:50000");
}

TEST_F(TestRemoteLogging, parseConfigGoodIpv6)
{
    std::string str = "*.* @@[abcd:ef01::01]:50000";
    std::stringstream ss(str);
    auto ret = phosphor::rsyslog_config::internal::parseConfig(ss);

    ASSERT_TRUE(ret.has_value());

    EXPECT_EQ(std::get<0>(*ret), "abcd:ef01::01");
    EXPECT_EQ(std::get<1>(*ret), 50000);
}

TEST_F(TestRemoteLogging, parseConfigBadIpv6WithoutRightBracket)
{
    std::string str = "*.* @@[abcd:ef01::01:50000";
    std::stringstream ss(str);
    auto ret = phosphor::rsyslog_config::internal::parseConfig(ss);
    EXPECT_FALSE(ret.has_value());
}

TEST_F(TestRemoteLogging, parseConfigBadIpv6WithoutLeftBracket)
{
    std::string str = "*.* @@abcd:ef01::01]:50000";
    std::stringstream ss(str);
    auto ret = phosphor::rsyslog_config::internal::parseConfig(ss);
    EXPECT_FALSE(ret.has_value());
}

TEST_F(TestRemoteLogging, parseConfigBadIpv6WithoutPort)
{
    std::string str = "*.* @@[abcd:ef01::01]:";
    std::stringstream ss(str);
    auto ret = phosphor::rsyslog_config::internal::parseConfig(ss);
    EXPECT_FALSE(ret.has_value());
}

TEST_F(TestRemoteLogging, parseConfigBadIpv6InvalidPort)
{
    std::string str = "*.* @@[abcd:ef01::01]:xxx";
    std::stringstream ss(str);
    auto ret = phosphor::rsyslog_config::internal::parseConfig(ss);
    EXPECT_FALSE(ret.has_value());
}

TEST_F(TestRemoteLogging, parseConfigBadIpv6WihtoutColon)
{
    std::string str = "*.* @@[abcd:ef01::01]";
    std::stringstream ss(str);
    auto ret = phosphor::rsyslog_config::internal::parseConfig(ss);
    EXPECT_FALSE(ret.has_value());
}

TEST_F(TestRemoteLogging, parseConfigBadEmpty)
{
    std::string str = "";
    std::stringstream ss(str);
    auto ret = phosphor::rsyslog_config::internal::parseConfig(ss);
    EXPECT_FALSE(ret.has_value());
}

TEST_F(TestRemoteLogging, parseConfigUdp)
{
    std::string str = "*.* @[abcd:ef01::01]:50000";
    std::stringstream ss(str);
    auto ret = phosphor::rsyslog_config::internal::parseConfig(ss);

    ASSERT_TRUE(ret.has_value());

    EXPECT_EQ(
        std::get<2>(*ret),
        phosphor::rsyslog_config::NetworkClient::TransportProtocol::UDP);
}

TEST_F(TestRemoteLogging, createUdpConfig)
{
    config->address("abcd:ef01::01");
    config->port(50000);
    config->transportProtocol(
        phosphor::rsyslog_config::NetworkClient::TransportProtocol::UDP);

    EXPECT_EQ(getConfig(configFilePath.c_str()),
              "*.* @[abcd:ef01::01]:50000");
}

} // namespace test
} // namespace logging
} // namespace phosphor
