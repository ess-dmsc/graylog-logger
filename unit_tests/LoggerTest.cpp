//
//  LoggerTest.cpp
//  graylog-logger
//
//  Copyright © 2022 European Spallation Source. All rights reserved.
//

#include "graylog_logger/Logger.hpp"
#include "graylog_logger/ConsoleInterface.hpp"
#include <ciso646>
#include <gtest/gtest.h>
#include <thread>

class LoggerStandIn : public Log::Logger {
public:
  LoggerStandIn() : Log::Logger() {}
};

using std::chrono_literals::operator""ms;

TEST(Logger, ReplaceConsoleInterface) {
  testing::internal::CaptureStdout();
  {
    LoggerStandIn UnderTest;
    UnderTest.log(Log::Severity::Error, "Some error string.");
    {
      auto CInterface = std::make_shared<Log::ConsoleInterface>();
      CInterface->setMessageStringCreatorFunction(
          [](auto &) { return "Static string"; });
      UnderTest.addLogHandler(CInterface);
    }
    UnderTest.log(Log::Severity::Error, "Some other error string.");
  }
  auto StdString = testing::internal::GetCapturedStdout();
  EXPECT_NE(StdString.find("Some error string."), std::string::npos);
  EXPECT_EQ(StdString.find("Some other error string."), std::string::npos);
  EXPECT_NE(StdString.find("Static string"), std::string::npos);
  EXPECT_GT(StdString.find("Static string"),
            StdString.find("Some error string."));
}
