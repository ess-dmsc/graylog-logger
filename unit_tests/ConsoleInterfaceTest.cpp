//
//  ConsoleInterfaceTest.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-21.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include "graylog_logger/ConsoleInterface.hpp"
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
