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
#include <ciso646>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace Log;

class ConsoleInterfaceStandIn : public ConsoleInterface {
public:
  ConsoleInterfaceStandIn(int queueSize) : ConsoleInterface(queueSize){};
  using ConsoleInterface::exitThread;
};

class FileInterfaceStandIn : public FileInterface {
public:
  FileInterfaceStandIn(int queueSize)
      : FileInterface("messages.log", queueSize){};
  using FileInterface::exitThread;
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

void TestFunc(BaseLogHandler *basePtr, int testLimit) {
  LogMessage usedMsg = GetLogMsg();
  for (int i = 0; i < testLimit; i++) {
    basePtr->addMessage(usedMsg);
    ASSERT_EQ(i + 1, basePtr->queueSize());
  }
  for (int u = 0; u < 10; u++) {
    basePtr->addMessage(usedMsg);
    ASSERT_EQ(testLimit, basePtr->queueSize());
  }
}

TEST_F(QueueLength, ConsoleInterfaceTest) {
  int queueLength = 50;
  auto console = std::make_unique<ConsoleInterfaceStandIn>(queueLength);
  console->exitThread();
  TestFunc((BaseLogHandler *)console.get(), queueLength);
}

TEST_F(QueueLength, FileInterfaceTest) {
  int queueLength = 50;
  auto file = std::make_unique<FileInterfaceStandIn>(queueLength);
  file->exitThread();
  TestFunc((BaseLogHandler *)file.get(), queueLength);
}

TEST_F(QueueLength, GraylogInterfaceTest) {
  int queueLength = 50;
  auto file =
      std::make_unique<GraylogInterface>("some_addr", 22222, queueLength);
  TestFunc((BaseLogHandler *)file.get(), queueLength);
}
