//
//  LoggingBaseTest.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-20.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include <boost/asio.hpp>
#include <chrono>
#include <ciso646>
#include <gtest/gtest.h>
#include <thread>
#include "BaseLogHandlerStandIn.hpp"
#include "graylog_logger/LogUtil.hpp"
#include "graylog_logger/LoggingBase.hpp"

class LoggingBaseStandIn : public LoggingBase {
public:
  using LoggingBase::baseMsg;
};

TEST(LoggingBase, InitTest) {
  LoggingBase log;
  ASSERT_EQ(log.GetHandlers().size(), 0);
}

TEST(LoggingBase, AddHandlerTest) {
  LoggingBase log;
  auto standIn = std::make_shared<BaseLogHandlerStandIn>();
  log.AddLogHandler(standIn);
  ASSERT_EQ(log.GetHandlers().size(), 1);
  auto handlers = log.GetHandlers();
  ASSERT_EQ(handlers[0].get(), standIn.get());
}

TEST(LoggingBase, ClearHandlersTest) {
  LoggingBase log;
  auto standIn = std::make_shared<BaseLogHandlerStandIn>();
  log.AddLogHandler(standIn);
  ASSERT_EQ(log.GetHandlers().size(), 1);
  log.RemoveAllHandlers();
  ASSERT_EQ(log.GetHandlers().size(), 0);
}

TEST(LoggingBase, LogSeveritiesTest) {
  LoggingBase log;
  log.SetMinSeverity(Severity::Debug);
  auto standIn = std::make_shared<BaseLogHandlerStandIn>();
  log.AddLogHandler(standIn);
  std::vector<Severity> testSeverities = {
      Severity::Alert,     Severity::Critical, Severity::Debug,
      Severity::Emergency, Severity::Error,    Severity::Informational,
      Severity::Notice,    Severity::Warning};
  for (auto sev : testSeverities) {
    log.Log(sev, "");
    ASSERT_EQ(standIn->cMsg.severity, sev);
  }
}

TEST(LoggingBase, LogIntSeveritiesTest) {
  LoggingBase log;
  log.SetMinSeverity(Severity::Debug);
  auto standIn = std::make_shared<BaseLogHandlerStandIn>();
  log.AddLogHandler(standIn);
  std::vector<Severity> testSeverities = {
      Severity::Alert,     Severity::Critical, Severity::Debug,
      Severity::Emergency, Severity::Error,    Severity::Informational,
      Severity::Notice,    Severity::Warning};
  for (auto sev : testSeverities) {
    log.Log(Severity(int(sev)), "");
    ASSERT_EQ(standIn->cMsg.severity, sev);
  }
  int testIntSev = -7;
  auto testSev = Severity(testIntSev);
  log.Log(testSev, "");
  ASSERT_EQ(standIn->cMsg.severity, Severity(testIntSev));
}

TEST(LoggingBase, LogMessageTest) {
  LoggingBase log;
  auto standIn = std::make_shared<BaseLogHandlerStandIn>();
  log.AddLogHandler(standIn);
  std::string baseStr("abcdefghijklmnopqrstuvwxyz\n"), tmpStr;

  for (int i = 0; i < 100; i++) {
    tmpStr += baseStr;
    log.Log(Severity::Critical, tmpStr);
    ASSERT_EQ(tmpStr, standIn->cMsg.message);
  }
}

TEST(LoggingBase, SetExtraField) {
  LoggingBaseStandIn log;
  std::string someKey = "yet_another_key";
  double someValue = -13.543462;
  log.AddField(someKey, someValue);
  ASSERT_EQ(log.baseMsg.additionalFields.size(), 1);
  ASSERT_EQ(log.baseMsg.additionalFields[0].first, someKey);
  ASSERT_EQ(log.baseMsg.additionalFields[0].second.FieldType,
            AdditionalField::Type::typeDbl);
  ASSERT_EQ(log.baseMsg.additionalFields[0].second.dblVal, someValue);
}

TEST(LoggingBase, LogMsgWithoutStaticExtraField) {
  LoggingBase log;
  auto standIn = std::make_shared<BaseLogHandlerStandIn>();
  log.AddLogHandler(standIn);
  log.Log(Severity::Alert, "Some message");
  ASSERT_EQ(standIn->cMsg.additionalFields.size(), 0);
}

TEST(LoggingBase, LogMsgWithStaticExtraField) {
  LoggingBase log;
  auto standIn = std::make_shared<BaseLogHandlerStandIn>();
  log.AddLogHandler(standIn);
  std::string someStaticExtraField = "some_key";
  std::int64_t someStaticExtraValue = -42344093;
  log.AddField(someStaticExtraField, someStaticExtraValue);
  log.Log(Severity::Alert, "Some message");
  ASSERT_EQ(standIn->cMsg.additionalFields.size(), 1);
  ASSERT_EQ(standIn->cMsg.additionalFields[0].first, someStaticExtraField);
  ASSERT_EQ(standIn->cMsg.additionalFields[0].second.FieldType,
            AdditionalField::Type::typeInt);
  ASSERT_EQ(standIn->cMsg.additionalFields[0].second.intVal,
            someStaticExtraValue);
}

TEST(LoggingBase, LogMsgWithDynamicExtraField) {
  LoggingBase log;
  auto standIn = std::make_shared<BaseLogHandlerStandIn>();
  log.AddLogHandler(standIn);
  std::string someStaticExtraField = "some_key";
  std::int64_t someStaticExtraValue = -42344093;
  log.Log(Severity::Alert, "Some message",
          {someStaticExtraField, someStaticExtraValue});
  ASSERT_EQ(standIn->cMsg.additionalFields.size(), 1);
  ASSERT_EQ(standIn->cMsg.additionalFields[0].first, someStaticExtraField);
  ASSERT_EQ(standIn->cMsg.additionalFields[0].second.FieldType,
            AdditionalField::Type::typeInt);
  ASSERT_EQ(standIn->cMsg.additionalFields[0].second.intVal,
            someStaticExtraValue);
}

TEST(LoggingBase, LogMsgWithTwoDynamicExtraFields) {
  LoggingBase log;
  auto standIn = std::make_shared<BaseLogHandlerStandIn>();
  log.AddLogHandler(standIn);
  std::string f1 = "key1";
  std::string f2 = "key2";
  std::int64_t v1 = -4234324123;
  std::string v2 = "value2";
  log.Log(Severity::Alert, "Some message", {{f1, v1}, {f2, v2}});
  ASSERT_EQ(standIn->cMsg.additionalFields.size(), 2);
  ASSERT_EQ(standIn->cMsg.additionalFields[0].first, f1);
  ASSERT_EQ(standIn->cMsg.additionalFields[0].second.FieldType,
            AdditionalField::Type::typeInt);
  ASSERT_EQ(standIn->cMsg.additionalFields[0].second.intVal, v1);

  ASSERT_EQ(standIn->cMsg.additionalFields[1].first, f2);
  ASSERT_EQ(standIn->cMsg.additionalFields[1].second.FieldType,
            AdditionalField::Type::typeStr);
  ASSERT_EQ(standIn->cMsg.additionalFields[1].second.strVal, v2);
}

TEST(LoggingBase, LogMsgWithTwoDynamicOverlappingExtraFields) {
  LoggingBase log;
  auto standIn = std::make_shared<BaseLogHandlerStandIn>();
  log.AddLogHandler(standIn);
  std::string f1 = "key1";
  std::int64_t v1 = -4234324123;
  std::string v2 = "value2";
  log.Log(Severity::Alert, "Some message", {{f1, v1}, {f1, v2}});
  ASSERT_EQ(standIn->cMsg.additionalFields.size(), 1);
  ASSERT_EQ(standIn->cMsg.additionalFields[0].first, f1);
  ASSERT_EQ(standIn->cMsg.additionalFields[0].second.FieldType,
            AdditionalField::Type::typeStr);
  ASSERT_EQ(standIn->cMsg.additionalFields[0].second.strVal, v2);
}

TEST(LoggingBase, LogMsgWithOverlappingStatDynExtraFields) {
  LoggingBase log;
  auto standIn = std::make_shared<BaseLogHandlerStandIn>();
  log.AddLogHandler(standIn);
  std::string f1 = "key1";
  std::int64_t v1 = -4234324123;
  std::string v2 = "value2";
  log.AddField(f1, v2);
  log.Log(Severity::Alert, "Some message", {f1, v1});
  ASSERT_EQ(standIn->cMsg.additionalFields.size(), 1);
  ASSERT_EQ(standIn->cMsg.additionalFields[0].first, f1);
  ASSERT_EQ(standIn->cMsg.additionalFields[0].second.FieldType,
            AdditionalField::Type::typeInt);
  ASSERT_EQ(standIn->cMsg.additionalFields[0].second.intVal, v1);
}

TEST(LoggingBase, MachineInfoTest) {
  LoggingBase log;
  auto standIn = std::make_shared<BaseLogHandlerStandIn>();
  log.AddLogHandler(standIn);
  log.Log(Severity::Critical, "No message");
  LogMessage msg = standIn->cMsg;
  ASSERT_EQ(msg.host, boost::asio::ip::host_name()) << "Incorrect host name.";
  std::ostringstream ss;
  ss << std::this_thread::get_id();
  ASSERT_EQ(msg.threadId, ss.str()) << "Incorrect thread id.";
  ASSERT_EQ(msg.processId, getpid()) << "Incorrect process id.";
  // ASSERT_EQ(msg.processName, boost::log::aux::get_process_name()) <<
  // "Incorrect process name.";
}

TEST(LoggingBase, TimestampTest) {
  LoggingBase log;
  auto standIn = std::make_shared<BaseLogHandlerStandIn>();
  log.AddLogHandler(standIn);
  log.Log(Severity::Critical, "No message");
  LogMessage msg = standIn->cMsg;
  std::chrono::duration<double> time_diff =
      std::chrono::system_clock::now() - msg.timestamp;
  ASSERT_NEAR(time_diff.count(), 0.0, 0.1) << "Time stamp is incorrect.";
}
