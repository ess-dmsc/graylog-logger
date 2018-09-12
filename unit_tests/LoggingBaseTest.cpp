//
//  LoggingBaseTest.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-20.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include "BaseLogHandlerStandIn.hpp"
#include "graylog_logger/LogUtil.hpp"
#include "graylog_logger/LoggingBase.hpp"
#include <asio.hpp>
#include <chrono>
#include <ciso646>
#include <gtest/gtest.h>
#include <thread>

class LoggingBaseStandIn : public LoggingBase {
public:
  using LoggingBase::BaseMsg;
};

TEST(LoggingBase, InitTest) {
  LoggingBase log;
  ASSERT_EQ(log.getHandlers().size(), 0);
}

TEST(LoggingBase, AddHandlerTest) {
  LoggingBase log;
  auto standIn = std::make_shared<BaseLogHandlerStandIn>();
  log.addLogHandler(standIn);
  ASSERT_EQ(log.getHandlers().size(), 1);
  auto handlers = log.getHandlers();
  ASSERT_EQ(handlers[0].get(), standIn.get());
}

TEST(LoggingBase, ClearHandlersTest) {
  LoggingBase log;
  auto standIn = std::make_shared<BaseLogHandlerStandIn>();
  log.addLogHandler(standIn);
  ASSERT_EQ(log.getHandlers().size(), 1);
  log.removeAllHandlers();
  ASSERT_EQ(log.getHandlers().size(), 0);
}

TEST(LoggingBase, LogSeveritiesTest) {
  LoggingBase log;
  log.setMinSeverity(Severity::Debug);
  auto standIn = std::make_shared<BaseLogHandlerStandIn>();
  log.addLogHandler(standIn);
  std::vector<Severity> testSeverities = {
      Severity::Alert,     Severity::Critical, Severity::Debug,
      Severity::Emergency, Severity::Error,    Severity::Informational,
      Severity::Notice,    Severity::Warning};
  for (auto sev : testSeverities) {
    log.log(sev, "");
    ASSERT_EQ(standIn->CurrentMessage.SeverityLevel, sev);
  }
}

TEST(LoggingBase, LogIntSeveritiesTest) {
  LoggingBase log;
  log.setMinSeverity(Severity::Debug);
  auto standIn = std::make_shared<BaseLogHandlerStandIn>();
  log.addLogHandler(standIn);
  std::vector<Severity> testSeverities = {
      Severity::Alert,     Severity::Critical, Severity::Debug,
      Severity::Emergency, Severity::Error,    Severity::Informational,
      Severity::Notice,    Severity::Warning};
  for (auto sev : testSeverities) {
    log.log(Severity(int(sev)), "");
    ASSERT_EQ(standIn->CurrentMessage.SeverityLevel, sev);
  }
  int testIntSev = -7;
  auto testSev = Severity(testIntSev);
  log.log(testSev, "");
  ASSERT_EQ(standIn->CurrentMessage.SeverityLevel, Severity(testIntSev));
}

TEST(LoggingBase, LogMessageTest) {
  LoggingBase log;
  auto standIn = std::make_shared<BaseLogHandlerStandIn>();
  log.addLogHandler(standIn);
  std::string baseStr("abcdefghijklmnopqrstuvwxyz\n"), tmpStr;

  for (int i = 0; i < 100; i++) {
    tmpStr += baseStr;
    log.log(Severity::Critical, tmpStr);
    ASSERT_EQ(tmpStr, standIn->CurrentMessage.MessageString);
  }
}

TEST(LoggingBase, SetExtraField) {
  LoggingBaseStandIn log;
  std::string someKey = "yet_another_key";
  double someValue = -13.543462;
  log.addField(someKey, someValue);
  ASSERT_EQ(log.BaseMsg.AdditionalFields.size(), 1);
  ASSERT_EQ(log.BaseMsg.AdditionalFields[0].first, someKey);
  ASSERT_EQ(log.BaseMsg.AdditionalFields[0].second.FieldType,
            AdditionalField::Type::typeDbl);
  ASSERT_EQ(log.BaseMsg.AdditionalFields[0].second.dblVal, someValue);
}

TEST(LoggingBase, LogMsgWithoutStaticExtraField) {
  LoggingBase log;
  auto standIn = std::make_shared<BaseLogHandlerStandIn>();
  log.addLogHandler(standIn);
  log.log(Severity::Alert, "Some message");
  ASSERT_EQ(standIn->CurrentMessage.AdditionalFields.size(), 0);
}

TEST(LoggingBase, LogMsgWithStaticExtraField) {
  LoggingBase log;
  auto standIn = std::make_shared<BaseLogHandlerStandIn>();
  log.addLogHandler(standIn);
  std::string someStaticExtraField = "some_key";
  std::int64_t someStaticExtraValue = -42344093;
  log.addField(someStaticExtraField, someStaticExtraValue);
  log.log(Severity::Alert, "Some message");
  ASSERT_EQ(standIn->CurrentMessage.AdditionalFields.size(), 1);
  ASSERT_EQ(standIn->CurrentMessage.AdditionalFields[0].first,
            someStaticExtraField);
  ASSERT_EQ(standIn->CurrentMessage.AdditionalFields[0].second.FieldType,
            AdditionalField::Type::typeInt);
  ASSERT_EQ(standIn->CurrentMessage.AdditionalFields[0].second.intVal,
            someStaticExtraValue);
}

TEST(LoggingBase, LogMsgWithDynamicExtraField) {
  LoggingBase log;
  auto standIn = std::make_shared<BaseLogHandlerStandIn>();
  log.addLogHandler(standIn);
  std::string someStaticExtraField = "some_key";
  std::int64_t someStaticExtraValue = -42344093;
  log.log(Severity::Alert, "Some message",
          {someStaticExtraField, someStaticExtraValue});
  ASSERT_EQ(standIn->CurrentMessage.AdditionalFields.size(), 1);
  ASSERT_EQ(standIn->CurrentMessage.AdditionalFields[0].first,
            someStaticExtraField);
  ASSERT_EQ(standIn->CurrentMessage.AdditionalFields[0].second.FieldType,
            AdditionalField::Type::typeInt);
  ASSERT_EQ(standIn->CurrentMessage.AdditionalFields[0].second.intVal,
            someStaticExtraValue);
}

TEST(LoggingBase, LogMsgWithTwoDynamicExtraFields) {
  LoggingBase log;
  auto standIn = std::make_shared<BaseLogHandlerStandIn>();
  log.addLogHandler(standIn);
  std::string f1 = "key1";
  std::string f2 = "key2";
  std::int64_t v1 = -4234324123;
  std::string v2 = "value2";
  log.log(Severity::Alert, "Some message", {{f1, v1}, {f2, v2}});
  ASSERT_EQ(standIn->CurrentMessage.AdditionalFields.size(), 2);
  ASSERT_EQ(standIn->CurrentMessage.AdditionalFields[0].first, f1);
  ASSERT_EQ(standIn->CurrentMessage.AdditionalFields[0].second.FieldType,
            AdditionalField::Type::typeInt);
  ASSERT_EQ(standIn->CurrentMessage.AdditionalFields[0].second.intVal, v1);

  ASSERT_EQ(standIn->CurrentMessage.AdditionalFields[1].first, f2);
  ASSERT_EQ(standIn->CurrentMessage.AdditionalFields[1].second.FieldType,
            AdditionalField::Type::typeStr);
  ASSERT_EQ(standIn->CurrentMessage.AdditionalFields[1].second.strVal, v2);
}

TEST(LoggingBase, LogMsgWithTwoDynamicOverlappingExtraFields) {
  LoggingBase log;
  auto standIn = std::make_shared<BaseLogHandlerStandIn>();
  log.addLogHandler(standIn);
  std::string f1 = "key1";
  std::int64_t v1 = -4234324123;
  std::string v2 = "value2";
  log.log(Severity::Alert, "Some message", {{f1, v1}, {f1, v2}});
  ASSERT_EQ(standIn->CurrentMessage.AdditionalFields.size(), 1);
  ASSERT_EQ(standIn->CurrentMessage.AdditionalFields[0].first, f1);
  ASSERT_EQ(standIn->CurrentMessage.AdditionalFields[0].second.FieldType,
            AdditionalField::Type::typeStr);
  ASSERT_EQ(standIn->CurrentMessage.AdditionalFields[0].second.strVal, v2);
}

TEST(LoggingBase, LogMsgWithOverlappingStatDynExtraFields) {
  LoggingBase log;
  auto standIn = std::make_shared<BaseLogHandlerStandIn>();
  log.addLogHandler(standIn);
  std::string f1 = "key1";
  std::int64_t v1 = -4234324123;
  std::string v2 = "value2";
  log.addField(f1, v2);
  log.log(Severity::Alert, "Some message", {f1, v1});
  ASSERT_EQ(standIn->CurrentMessage.AdditionalFields.size(), 1);
  ASSERT_EQ(standIn->CurrentMessage.AdditionalFields[0].first, f1);
  ASSERT_EQ(standIn->CurrentMessage.AdditionalFields[0].second.FieldType,
            AdditionalField::Type::typeInt);
  ASSERT_EQ(standIn->CurrentMessage.AdditionalFields[0].second.intVal, v1);
}

TEST(LoggingBase, MachineInfoTest) {
  LoggingBase log;
  auto standIn = std::make_shared<BaseLogHandlerStandIn>();
  log.addLogHandler(standIn);
  log.log(Severity::Critical, "No message");
  LogMessage msg = standIn->CurrentMessage;
  ASSERT_EQ(msg.Host, asio::ip::host_name()) << "Incorrect host name.";
  std::ostringstream ss;
  ss << std::this_thread::get_id();
  ASSERT_EQ(msg.ThreadId, ss.str()) << "Incorrect thread id.";
  ASSERT_EQ(msg.ProcessId, getpid()) << "Incorrect process id.";
}

TEST(LoggingBase, TimestampTest) {
  LoggingBase log;
  auto standIn = std::make_shared<BaseLogHandlerStandIn>();
  log.addLogHandler(standIn);
  log.log(Severity::Critical, "No message");
  LogMessage msg = standIn->CurrentMessage;
  std::chrono::duration<double> time_diff =
      std::chrono::system_clock::now() - msg.Timestamp;
  ASSERT_NEAR(time_diff.count(), 0.0, 0.1) << "Time stamp is incorrect.";
}
