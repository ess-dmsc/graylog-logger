//
//  ConsoleInterfaceTest.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-21.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include "graylog_logger/ConsoleInterface.hpp"
#include "Semaphore.hpp"
#include "graylog_logger/LoggingBase.hpp"
#include <atomic>
#include <ciso646>
#include <gtest/gtest.h>

using namespace Log;

const std::string consoleTestString("Some test string");

std::string ConsoleTestStringCreator(const LogMessage &msg) {
  return consoleTestString;
}

TEST(ConsoleInterface, ConsoleStringTest) {
  testing::internal::CaptureStdout();
  {
    LogMessage Message;
    ConsoleInterface cInter;
    cInter.setMessageStringCreatorFunction(ConsoleTestStringCreator);
    cInter.addMessage(Message);
  }
  std::string output = testing::internal::GetCapturedStdout();
  ASSERT_EQ(output, consoleTestString + "\n");
}

TEST(ConsoleInterface, ConsoleStringFunctionTest) {
  testing::internal::CaptureStdout();
  {
    LogMessage Message;
    Message.SeverityLevel = Severity::Alert;
    Message.MessageString = consoleTestString;
    ConsoleInterface cInter;
    cInter.addMessage(Message);
  }
  std::string output = testing::internal::GetCapturedStdout();
  ASSERT_EQ(std::string("ALERT: Some test string\n"), output);
}

TEST(ConsoleInterface, OnInitialisationQueueEmpty) {
  ConsoleInterface cInter;
  ASSERT_EQ(cInter.queueSize(), 0);
  ASSERT_TRUE(cInter.emptyQueue());
}

class ConsoleInterfaceStandIn : public ConsoleInterface {
public:
  void addMessage(LogMessage const &) override { GotMsg = true; }
  std::atomic_bool GotMsg{false};
  using ConsoleInterface::Executor;
};

class TempLoggingBase : public LoggingBase {
public:
  using LoggingBase::Executor;
};

TEST(LoggingBase, AddConsoleHandlerTest) {
  TempLoggingBase log;
  auto standIn = std::make_shared<ConsoleInterfaceStandIn>();
  ASSERT_EQ(standIn.use_count(), 1);
  log.addLogHandler(standIn);
  ASSERT_EQ(standIn.use_count(), 2);
  ASSERT_FALSE(standIn->GotMsg);
  log.log({}, "Msg");
  Semaphore Signal;
  log.Executor.SendWork([&]() { Signal.notify(); });
  Signal.wait();
  ASSERT_TRUE(standIn->GotMsg);
}

TEST(ConsoleInterface, QueueSizeOneIsNotEmpty) {
  ConsoleInterfaceStandIn cInter;
  Semaphore Signal1, Signal2;
  Semaphore Signal3;
  cInter.Executor.SendWork([&]() {
    Signal1.notify();
    Signal2.wait();
    Signal3.notify();
  });
  cInter.Executor.SendWork([]() {});
  Signal1.wait();
  EXPECT_EQ(cInter.queueSize(), 1);
  EXPECT_FALSE(cInter.emptyQueue());
  Signal2.notify();
  Signal3.wait();
}

using namespace std::chrono_literals;

TEST(ConsoleInterface, FlushSuccess) {
  ConsoleInterfaceStandIn cInter;
  EXPECT_TRUE(cInter.flush(50ms));
}

TEST(ConsoleInterface, FlushFail) {
  ConsoleInterfaceStandIn cInter;
  Semaphore Signal1, Signal2;
  cInter.Executor.SendWork([&]() {
    Signal1.wait();
    Signal2.notify();
  });
  EXPECT_FALSE(cInter.flush(50ms));
  Signal1.notify();
  Signal2.wait();
}
