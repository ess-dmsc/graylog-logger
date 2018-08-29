//
//  BaseLogHandlerTest.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-20.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include "BaseLogHandlerStandIn.hpp"
#include <ciso646>
#include <gtest/gtest.h>
#include <regex>

const std::string testString("Some test string");

std::string MyStringCreator(const LogMessage &msg) { return testString; }

TEST(BaseLogHandler, DefaultStringCreatorTest) {
  LogMessage msg;
  msg.timestamp = std::chrono::system_clock::now();
  msg.message = testString;
  msg.host = "Nohost";
  msg.severity = Severity::Alert;
  BaseLogHandlerStandIn standIn;
  std::string logString = standIn.MsgStringCreator(msg);
  std::regex exp("\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2} \\(Nohost\\) "
                 "ALERT: Some test string");
  ASSERT_TRUE(std::regex_match(logString.c_str(), exp));
}

TEST(BaseLogHandler, SetStringCreatorTest) {
  LogMessage msg;
  BaseLogHandlerStandIn standIn;
  ASSERT_FALSE(standIn.MessageParser);
  standIn.SetMessageStringCreatorFunction(MyStringCreator);
  ASSERT_TRUE(standIn.MessageParser);
}

TEST(BaseLogHandler, NewStringCreatorTest) {
  LogMessage msg;
  BaseLogHandlerStandIn standIn;
  standIn.SetMessageStringCreatorFunction(&MyStringCreator);
  ASSERT_EQ(standIn.MsgStringCreator(msg), testString);
}
