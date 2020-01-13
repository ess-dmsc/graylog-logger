//
//  GraylogInterfaceTest.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-21.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include "graylog_logger/GraylogInterface.hpp"
#include "LogTestServer.hpp"
#include "Semaphore.hpp"
#include <ciso646>
#include <cmath>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <nlohmann/json.hpp>
#include <thread>

using namespace Log;

MATCHER(IsJSON, "") {
  try {
    auto Res = nlohmann::json::parse(arg);
  } catch (std::exception const &e) {
    return false;
  }
  return true;
}

class GraylogInterfaceStandIn : public GraylogInterface {
public:
  GraylogInterfaceStandIn(std::string host, int port, int queueLength)
      : GraylogInterface(host, port, queueLength){};
  MOCK_METHOD1(sendMessage, void(std::string));
  using GraylogInterface::logMsgToJSON;
  void sendMessageBase(std::string Msg) {GraylogInterface::sendMessage(Msg);}
};

class GraylogConnectionStandIn : public GraylogConnection {
public:
  GraylogConnectionStandIn(std::string host, int port, int queueLength = 100)
      : GraylogConnection(host, port, queueLength){};
  ~GraylogConnectionStandIn(){};
};

const int testPort = 2526;
const std::chrono::milliseconds sleepTime(100);

class GraylogConnectionCom : public ::testing::Test {
public:
  static void SetUpTestCase() {
    logServer = std::make_unique<LogTestServer>(testPort);
  };

  static void TearDownTestCase() { logServer.reset(); };

  virtual void SetUp(){};

  virtual void TearDown() {
    logServer->CloseAllConnections();
    logServer->GetLastSocketError();
    logServer->GetLatestMessage();
    logServer->ClearReceivedBytes();
  };

  static std::unique_ptr<LogTestServer> logServer;
};

std::unique_ptr<LogTestServer> GraylogConnectionCom::logServer;

TEST_F(GraylogConnectionCom, UnknownHostTest) {
  GraylogConnectionStandIn con("no_host", testPort);
  std::this_thread::sleep_for(sleepTime);
  ASSERT_EQ(logServer->GetNrOfConnections(), 0);
  ASSERT_EQ(logServer->GetLatestMessage().size(), 0);
  ASSERT_TRUE(!logServer->GetLastSocketError());
  EXPECT_NE(con.getConnectionStatus(), GraylogConnection::Status::SEND_LOOP);
}

TEST_F(GraylogConnectionCom, ConnectionTest) {
  ASSERT_EQ(0, logServer->GetNrOfConnections());
  ASSERT_EQ(0, logServer->GetLatestMessage().size());
  ASSERT_TRUE(!logServer->GetLastSocketError());
  {
    GraylogConnectionStandIn con("localhost", testPort);
    std::this_thread::sleep_for(sleepTime);
    ASSERT_EQ(1, logServer->GetNrOfConnections());
    ASSERT_EQ(logServer->GetLatestMessage().size(), 0);
    ASSERT_TRUE(!logServer->GetLastSocketError());
    ASSERT_EQ(GraylogConnection::Status::SEND_LOOP, con.getConnectionStatus())
        << "Connection status returned " << int(con.getConnectionStatus());
  }
  std::this_thread::sleep_for(sleepTime);
  ASSERT_EQ(0, logServer->GetNrOfConnections());
  ASSERT_EQ(0, logServer->GetLatestMessage().size());
  auto SocketError = logServer->GetLastSocketError();
#ifdef WIN32
  ASSERT_TRUE(SocketError == asio::error::connection_reset);
#else
  ASSERT_TRUE(SocketError == asio::error::misc_errors::eof);
#endif
}

TEST_F(GraylogConnectionCom, IPv6ConnectionTest) {
  ASSERT_EQ(0, logServer->GetNrOfConnections());
  ASSERT_EQ(0, logServer->GetLatestMessage().size());
  ASSERT_TRUE(!logServer->GetLastSocketError());
  {
    GraylogConnectionStandIn con("::1", testPort);
    std::this_thread::sleep_for(sleepTime);
    ASSERT_EQ(1, logServer->GetNrOfConnections());
    ASSERT_EQ(logServer->GetLatestMessage().size(), 0);
    ASSERT_TRUE(!logServer->GetLastSocketError());
    ASSERT_EQ(GraylogConnection::Status::SEND_LOOP, con.getConnectionStatus())
        << "Connection status returned " << int(con.getConnectionStatus());
  }
  std::this_thread::sleep_for(sleepTime);
  ASSERT_EQ(0, logServer->GetNrOfConnections());
  ASSERT_EQ(0, logServer->GetLatestMessage().size());
  auto SocketError = logServer->GetLastSocketError();
#ifdef WIN32
  ASSERT_TRUE(SocketError == asio::error::connection_reset);
#else
  ASSERT_TRUE(SocketError == asio::error::misc_errors::eof);
#endif
}

TEST_F(GraylogConnectionCom, WrongPortTest) {
  GraylogConnectionStandIn con("localhost", testPort + 1);
  std::this_thread::sleep_for(sleepTime);
  ASSERT_EQ(logServer->GetNrOfConnections(), 0);
  ASSERT_EQ(logServer->GetLatestMessage().size(), 0);
  ASSERT_TRUE(!logServer->GetLastSocketError());
  EXPECT_NE(con.getConnectionStatus(), GraylogConnection::Status::SEND_LOOP);
}

TEST_F(GraylogConnectionCom, CloseConnectionTest) {
  {
    GraylogConnectionStandIn con("localhost", testPort);
    std::this_thread::sleep_for(sleepTime);
    ASSERT_EQ(GraylogConnection::Status::SEND_LOOP, con.getConnectionStatus());
    ASSERT_EQ(1, logServer->GetNrOfConnections());
    logServer->CloseAllConnections();
    std::this_thread::sleep_for(sleepTime * 2);
    EXPECT_EQ(GraylogConnection::Status::SEND_LOOP, con.getConnectionStatus());
    EXPECT_EQ(1, logServer->GetNrOfConnections())
        << "Failed to reconnect after connection was closed remotely.";
  }
  std::this_thread::sleep_for(sleepTime);
  EXPECT_EQ(0, logServer->GetNrOfConnections());
}

TEST_F(GraylogConnectionCom, MessageTransmissionTest) {
  {
    std::string testString("This is a test string!");
    GraylogConnectionStandIn con("localhost", testPort);
    con.sendMessage(testString);
    std::this_thread::sleep_for(sleepTime);
    ASSERT_TRUE(!logServer->GetLastSocketError());
    ASSERT_EQ(testString.size() + 1, logServer->GetReceivedBytes());
    ASSERT_EQ(testString, logServer->GetLatestMessage());
    ASSERT_EQ(1, logServer->GetNrOfConnections());
  }
}

TEST_F(GraylogConnectionCom, DISABLED_LargeMessageTransmissionTest) {
  {
    std::string RepeatedString("This is a test string!");
    std::string TargetString;
    for (size_t i = 0; i < 30000; i++) {
      TargetString += RepeatedString;
    }
    GraylogConnectionStandIn con("localhost", testPort);
    con.sendMessage(TargetString);
    std::this_thread::sleep_for(sleepTime);
    ASSERT_TRUE(!logServer->GetLastSocketError());
    ASSERT_EQ(TargetString.size() + 1, logServer->GetReceivedBytes());
    ASSERT_EQ(TargetString, logServer->GetLatestMessage());
    ASSERT_EQ(1, logServer->GetNrOfConnections());
  }
}

TEST_F(GraylogConnectionCom, DISABLED_MultipleMessagesTest) {
  std::vector<std::string> lines = {"This is a test.", "!\"#€%&/()=?*^_-.,:;",
                                    "Another line bites the dust."};
  {
    GraylogConnectionStandIn con("localhost", testPort);
    std::this_thread::sleep_for(sleepTime);
    int totalBytes = 0;
    for (auto ln : lines) {
      totalBytes += (ln.size() + 1);
      con.sendMessage(ln);
    }
    std::this_thread::sleep_for(sleepTime);
    ASSERT_TRUE(!logServer->GetLastSocketError());
    ASSERT_EQ(lines[lines.size() - 1], logServer->GetLatestMessage());
    ASSERT_EQ(totalBytes, logServer->GetReceivedBytes());
    ASSERT_EQ(1, logServer->GetNrOfConnections());
  }
}

LogMessage GetPopulatedLogMsg() {
  LogMessage retMsg;
  retMsg.Host = "Some host";
  retMsg.MessageString =
      "This is some multi line\n error message with \"quotes\".";
  retMsg.ProcessId = 667;
  retMsg.ProcessName = "some_process_name";
  retMsg.SeverityLevel = Severity::Alert;
  retMsg.ThreadId = "0xff0011aacc";
  retMsg.Timestamp = std::chrono::system_clock::now();
  return retMsg;
}

TEST(GraylogInterfaceCom, AddMessageTest) {
  GraylogInterfaceStandIn con("localhost", testPort, 100);
  EXPECT_CALL(con, sendMessage(::testing::_)).Times(::testing::Exactly(1));
  LogMessage msg = GetPopulatedLogMsg();
  con.addMessage(msg);
}

TEST(GraylogInterfaceCom, MessageJSONTest) {
  LogMessage msg = GetPopulatedLogMsg();
  GraylogInterfaceStandIn con("localhost", testPort, 100);
  EXPECT_CALL(con, sendMessage(IsJSON())).Times(::testing::Exactly(1));
  con.addMessage(msg);
}

void TestJsonString(const std::string &jsonMsg) {
  auto JsonObject = nlohmann::json::parse(jsonMsg);
  LogMessage compLog = GetPopulatedLogMsg();
  std::string tempStr;
  double tempDouble = 0;
  int tempInt = 0;
  EXPECT_NO_THROW(tempStr = JsonObject["short_message"]);
  EXPECT_EQ(tempStr, compLog.MessageString);
  EXPECT_NO_THROW(tempDouble = JsonObject["timestamp"]);
  auto TempTS =
      static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(
                              compLog.Timestamp.time_since_epoch())
                              .count()) /
      1000;
  EXPECT_NEAR(tempDouble, TempTS, 0.01);
  EXPECT_NO_THROW(tempStr = JsonObject["host"]);
  EXPECT_EQ(tempStr, compLog.Host);
  EXPECT_NO_THROW(tempInt = JsonObject["_process_id"]);
  EXPECT_EQ(tempInt, compLog.ProcessId);
  EXPECT_NO_THROW(tempStr = JsonObject["_process"]);
  EXPECT_EQ(tempStr, compLog.ProcessName);
  EXPECT_NO_THROW(tempInt = JsonObject["level"]);
  EXPECT_EQ(tempInt, int(compLog.SeverityLevel));
  EXPECT_NO_THROW(tempStr = JsonObject["_thread_id"]);
  EXPECT_EQ(tempStr, compLog.ThreadId);
}

TEST(GraylogInterfaceCom, MessageJSONContentTest) {
  LogMessage msg = GetPopulatedLogMsg();
  GraylogInterfaceStandIn con("localhost", testPort, 100);
  EXPECT_CALL(con, sendMessage(::testing::_))
      .WillOnce(testing::Invoke(&TestJsonString));
  con.addMessage(msg);
}

TEST(GraylogInterfaceCom, TestAdditionalFieldString) {
  GraylogInterfaceStandIn con("localhost", testPort, 100);
  LogMessage testMsg = GetPopulatedLogMsg();
  std::string key = "yet_another_key";
  std::string value = "yet another value";
  testMsg.addField(key, value);
  std::string jsonStr = GraylogInterfaceStandIn::logMsgToJSON(testMsg);
  auto JsonObject = nlohmann::json::parse(jsonStr);
  std::string tempStr;
  EXPECT_NO_THROW(tempStr = JsonObject["_" + key]);
  EXPECT_EQ(tempStr, value);
}

TEST(GraylogInterfaceCom, TestAdditionalFieldInt) {
  GraylogInterfaceStandIn con("localhost", testPort, 100);
  LogMessage testMsg = GetPopulatedLogMsg();
  std::string key = "yet_another_key";
  std::int64_t value = -12431454;
  testMsg.addField(key, value);
  std::string jsonStr = GraylogInterfaceStandIn::logMsgToJSON(testMsg);
  auto JsonObject = nlohmann::json::parse(jsonStr);
  std::int64_t tempVal = 0;
  EXPECT_NO_THROW(tempVal = JsonObject["_" + key]);
  EXPECT_EQ(tempVal, value);
}

TEST(GraylogInterfaceCom, TestAdditionalFieldDouble) {
  GraylogInterfaceStandIn con("localhost", testPort, 100);
  LogMessage testMsg = GetPopulatedLogMsg();
  std::string key = "yet_another_key";
  double value = 3.1415926535897932384626433832795028841;
  testMsg.addField(key, value);
  std::string jsonStr = GraylogInterfaceStandIn::logMsgToJSON(testMsg);
  auto JsonObject = nlohmann::json::parse(jsonStr);
  double tempVal;
  EXPECT_NO_THROW(tempVal = JsonObject["_" + key]);
  EXPECT_EQ(tempVal, value);
}

TEST(GraylogInterfaceCom, TestQueueSize) {
  GraylogInterface con("localhost", testPort, 100);
  LogMessage testMsg = GetPopulatedLogMsg();
  con.addMessage(testMsg);
  EXPECT_EQ(con.queueSize(), 1);
}

TEST(GraylogInterfaceCom, TestQueueSizeLimit) {
  int MaxNrOfMessages = 64;
  GraylogInterface con("localhost", testPort, MaxNrOfMessages);
  LogMessage testMsg = GetPopulatedLogMsg();
  for (int i = 0; i < MaxNrOfMessages * 4; ++i) {
    con.addMessage(testMsg);
  }
  EXPECT_NEAR(con.queueSize(), MaxNrOfMessages, 20);
}
using std::chrono_literals::operator""ms;

TEST_F(GraylogConnectionCom, DISABLED_MultipleCloseConnectionTest) {
  {
    GraylogConnectionStandIn con("localhost", testPort);
    std::string SomeTestMessage(
        "Hello, this is some test message with the number ");
    int NrOfMessages = 100;
    for (int i = 0; i < NrOfMessages; ++i) {
      con.sendMessage(SomeTestMessage + std::to_string(i));
      logServer->CloseAllConnections();
      std::this_thread::sleep_for(20ms);
    }
    con.flush(10000ms);
    EXPECT_EQ(logServer->GetNrOfMessages(), NrOfMessages);
  }
}

TEST(GraylogInterface, QueueSizeEmpty) {
  GraylogInterfaceStandIn cInter("localhost", testPort, 10);
  ASSERT_EQ(cInter.queueSize(), 0);
  ASSERT_TRUE(cInter.emptyQueue());
}

TEST(GraylogInterface, QueueSizeOne) {
  GraylogInterfaceStandIn cInter("localhost", testPort, 10);
  cInter.sendMessageBase("tst_msg");
  cInter.sendMessageBase("tst_msg");
  EXPECT_GE(cInter.queueSize(), 1);
  EXPECT_LE(cInter.queueSize(), 2);
  EXPECT_FALSE(cInter.emptyQueue());
}

using namespace std::chrono_literals;

TEST(GraylogInterface, FlushSuccess) {
  auto LogServer = std::make_unique<LogTestServer>(testPort);
  GraylogInterfaceStandIn cInter("localhost", testPort, 10);
  EXPECT_TRUE(cInter.flush(50ms));
}

TEST(GraylogInterface, FlushFail) {
  GraylogInterfaceStandIn cInter("localhost", testPort, 10);
  cInter.sendMessageBase("tst_msg");
  EXPECT_FALSE(cInter.flush(50ms));
}
