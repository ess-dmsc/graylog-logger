//
//  QueueLengthTest.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2017-01-20.
//  Copyright © 2017 European Spallation Source. All rights reserved.
//

#include "graylog_logger/ConsoleInterface.hpp"
#include "graylog_logger/FileInterface.hpp"
#include "graylog_logger/GraylogInterface.hpp"
#include <atomic>
#include <ciso646>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace Log;

class ConsoleInterfaceStandIn2 : public ConsoleInterface {
public:
  ConsoleInterfaceStandIn2() : ConsoleInterface(){};
};

class FileInterfaceStandIn : public FileInterface {
public:
  FileInterfaceStandIn(int queueSize)
      : FileInterface("messages.log", queueSize){};
};

class QueueLength : public ::testing::Test {
public:
  static void SetUpTestCase(){};

  static void TearDownTestCase(){};

  virtual void SetUp(){};

  virtual void TearDown(){};
};

LogMessage GetLogMsg() {
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

TEST_F(QueueLength, ConsoleInterfaceTest) {
  std::atomic_int MsgCounter{0};
  int TestLimit{50};
  testing::internal::CaptureStdout();
  {
    ConsoleInterfaceStandIn2 CLogger;
    CLogger.setMessageStringCreatorFunction([&MsgCounter](auto Msg) {
      MsgCounter++;
      return "";
    });
    LogMessage usedMsg = GetLogMsg();
    for (int i = 0; i < TestLimit; i++) {
      CLogger.addMessage(usedMsg);
    }
  }
  testing::internal::GetCapturedStdout();
  EXPECT_EQ(MsgCounter, TestLimit);
}

TEST_F(QueueLength, FileInterfaceTest) {
  std::atomic_int MsgCounter{0};
  int QueueLength = 50;
  int TestLimit{50};
  testing::internal::CaptureStdout();
  {
    FileInterfaceStandIn CLogger(QueueLength);
    CLogger.setMessageStringCreatorFunction([&MsgCounter](auto Msg) {
      MsgCounter++;
      return "";
    });
    LogMessage usedMsg = GetLogMsg();
    for (int i = 0; i < TestLimit; i++) {
      CLogger.addMessage(usedMsg);
    }
  }
  testing::internal::GetCapturedStdout();
  EXPECT_EQ(MsgCounter, TestLimit);
}

TEST_F(QueueLength, GraylogInterfaceTest) {
  std::atomic_int MsgCounter{0};
  int QueueLength = 50;
  int TestLimit{50};
  {
    GraylogInterface CLogger("some_addr", 22222, QueueLength);
    CLogger.setMessageStringCreatorFunction([&MsgCounter](auto Msg) {
      MsgCounter++;
      return "";
    });
    LogMessage usedMsg = GetLogMsg();
    for (int i = 0; i < TestLimit; i++) {
      CLogger.addMessage(usedMsg);
    }
    EXPECT_EQ(CLogger.queueSize(), TestLimit);
  }
}
