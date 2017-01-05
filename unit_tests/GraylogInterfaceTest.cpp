//
//  GraylogInterfaceTest.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-21.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>
#include <sstream>
#include "graylog_logger/GraylogInterface.hpp"
#include "LogTestServer.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace boost::property_tree;

MATCHER(IsJSON, "") {
    std::stringstream ss;
    ss << arg;
    ptree pt;
    try {
        read_json(ss, pt);
    } catch (std::exception const& e) {
        return false;
    }
    return true;
}

class GraylogInterfaceStandIn : public GraylogInterface {
public:
    GraylogInterfaceStandIn(std::string host, int port, int queueLength) : GraylogInterface(host, port, queueLength) {
    };
    MOCK_METHOD1(SendMessage, void(std::string msg));
};

class GraylogConnectionStandIn : public GraylogConnection {
public:
    GraylogConnectionStandIn(std::string host, int port, int queueLength = 100) : GraylogConnection(host, port, queueLength) {};
    ~GraylogConnectionStandIn() {};
    using GraylogConnection::EndThread;
};

const int testPort = 2526;
const std::chrono::milliseconds sleepTime(100);

class GraylogConnectionCom : public ::testing::Test{
public:
    static void SetUpTestCase() {
        logServer = new LogTestServer(testPort);
    };
    
    static void TearDownTestCase() {
        delete logServer;
    };
    
    virtual void SetUp() {
    };
    
    virtual void TearDown() {
        logServer->CloseAllConnections();
        logServer->GetLastSocketError();
        logServer->GetLatestMessage();
        logServer->ClearReceivedBytes();
    };
    
    static LogTestServer *logServer;
};

LogTestServer *GraylogConnectionCom::logServer = NULL;

TEST_F(GraylogConnectionCom, UnknownHostTest) {
    GraylogConnectionStandIn con("no_host", testPort);
    std::this_thread::sleep_for(sleepTime);
    ASSERT_EQ(logServer->GetNrOfConnections(), 0);
    ASSERT_EQ(logServer->GetLatestMessage().size(), 0);
    ASSERT_EQ(logServer->GetLastSocketError(), int(errc_t::success));
}

TEST_F(GraylogConnectionCom, ConnectionTest) {
    ASSERT_EQ(0, logServer->GetNrOfConnections());
    ASSERT_EQ(0, logServer->GetLatestMessage().size());
    ASSERT_EQ(int(errc_t::success), logServer->GetLastSocketError());
    {
        GraylogConnectionStandIn con("localhost", testPort);
        std::this_thread::sleep_for(sleepTime);
        ASSERT_EQ(1, logServer->GetNrOfConnections());
        ASSERT_EQ(logServer->GetLatestMessage().size(), 0);
        ASSERT_EQ(logServer->GetLastSocketError(), int(errc_t::success));
        ASSERT_EQ(GraylogConnection::ConStatus::NEW_MESSAGE, con.GetConnectionStatus());
    }
    std::this_thread::sleep_for(sleepTime);
    ASSERT_EQ(0, logServer->GetNrOfConnections());
    ASSERT_EQ(0, logServer->GetLatestMessage().size());
    ASSERT_EQ(int(errc_t::success), logServer->GetLastSocketError());
}

TEST_F(GraylogConnectionCom, CloseConnectionTest) {
    {
        GraylogConnectionStandIn con("localhost", testPort);
        std::this_thread::sleep_for(sleepTime);
        ASSERT_EQ(GraylogConnection::ConStatus::NEW_MESSAGE, con.GetConnectionStatus());
        ASSERT_EQ(1, logServer->GetNrOfConnections());
        logServer->CloseAllConnections();
        std::this_thread::sleep_for(sleepTime);
        ASSERT_EQ(GraylogConnection::ConStatus::NEW_MESSAGE, con.GetConnectionStatus());
        ASSERT_EQ(1, logServer->GetNrOfConnections()) << "Failed to reconnect after connection was closed remotely.";
    }
    std::this_thread::sleep_for(sleepTime);
    ASSERT_EQ(0, logServer->GetNrOfConnections());
}

TEST_F(GraylogConnectionCom, MessageTransmissionTest) {
    {
        std::string testString("This is a test string!");
        GraylogConnectionStandIn con("localhost", testPort);
        con.SendMessage(testString);
        std::this_thread::sleep_for(sleepTime);
        ASSERT_EQ(int(errc_t::success),logServer->GetLastSocketError());
        ASSERT_EQ(testString, logServer->GetLatestMessage());
        ASSERT_EQ(testString.size() + 1, logServer->GetReceivedBytes());
        ASSERT_EQ(1, logServer->GetNrOfConnections());
    }
}

TEST_F(GraylogConnectionCom, MultipleMessagesTest) {
    std::vector<std::string> lines = {"This is a test.", "!\"#€%&/()=?*^_-.,:;", "Another line bites the dust."};
    {
        GraylogConnectionStandIn con("localhost", testPort);
        std::this_thread::sleep_for(sleepTime);
        int totalBytes = 0;
        for(auto ln : lines) {
            totalBytes += (ln.size() + 1); con.SendMessage(ln);
        }
        std::this_thread::sleep_for(sleepTime);
        ASSERT_EQ(int(errc_t::success),logServer->GetLastSocketError());
        ASSERT_EQ(lines[lines.size() - 1], logServer->GetLatestMessage());
        ASSERT_EQ(totalBytes, logServer->GetReceivedBytes());
        ASSERT_EQ(1, logServer->GetNrOfConnections());
    }
}

LogMessage GetPopulatedLogMsg() {
    LogMessage retMsg;
    retMsg.host = "Some host";
    retMsg.message = "This is some multi line\n error message with \"quotes\".";
    retMsg.processId = 667;
    retMsg.processName = "some_process_name";
    retMsg.severity = Severity::Alert;
    retMsg.threadId = "0xff0011aacc";
    retMsg.timestamp = std::chrono::system_clock::now();
    return retMsg;
}

TEST(GraylogInterfaceCom, AddMessageTest) {
    GraylogInterfaceStandIn con("localhost", testPort, 100);
    EXPECT_CALL(con, SendMessage(::testing::_)).Times(::testing::Exactly(1));
    LogMessage msg = GetPopulatedLogMsg();
    con.AddMessage(msg);
}

TEST(GraylogInterfaceCom, MessageJSONTest) {
    LogMessage msg = GetPopulatedLogMsg();
    GraylogInterfaceStandIn con("localhost", testPort, 100);
    EXPECT_CALL(con, SendMessage(IsJSON())).Times(::testing::Exactly(1));
    con.AddMessage(msg);
}

void TestJsonString(std::string jsonMsg) {
    std::stringstream ss;
    ss << jsonMsg;
    ptree pt;
    EXPECT_NO_THROW(read_json(ss, pt));
    LogMessage compLog = GetPopulatedLogMsg();
    std::string tempStr;
    double tempDouble;
    int tempInt;
    EXPECT_NO_THROW(tempStr = pt.get<std::string>("short_message"));
    EXPECT_EQ(tempStr, compLog.message);
    EXPECT_NO_THROW(tempDouble = pt.get<double>("timestamp"));
    EXPECT_NEAR(tempDouble, double(std::chrono::system_clock::to_time_t(compLog.timestamp)), 0.01);
    EXPECT_NO_THROW(tempStr = pt.get<std::string>("host"));
    EXPECT_EQ(tempStr, compLog.host);
    EXPECT_NO_THROW(tempInt = pt.get<int>("_process_id"));
    EXPECT_EQ(tempInt, compLog.processId);
    EXPECT_NO_THROW(tempStr = pt.get<std::string>("_process"));
    EXPECT_EQ(tempStr, compLog.processName);
    EXPECT_NO_THROW(tempInt = pt.get<int>("level"));
    EXPECT_EQ(tempInt, int(compLog.severity));
    EXPECT_NO_THROW(tempStr = pt.get<std::string>("_thread_id"));
    EXPECT_EQ(tempStr, compLog.threadId);
}

TEST(GraylogInterfaceCom, MessageJSONContentTest) {
    LogMessage msg = GetPopulatedLogMsg();
    GraylogInterfaceStandIn con("localhost", testPort, 100);
    EXPECT_CALL(con, SendMessage(::testing::_)).WillOnce(testing::Invoke(&TestJsonString));
    con.AddMessage(msg);
}
