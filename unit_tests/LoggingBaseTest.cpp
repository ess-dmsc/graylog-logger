//
//  LoggingBaseTest.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-20.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//


#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "BaseLogHandlerStandIn.hpp"
#include "graylog_logger/LoggingBase.hpp"
#include "graylog_logger/LogUtil.hpp"

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
    std::vector<Severity> testSeverities = {Severity::Alert, Severity::Critical, Severity::Debug, Severity::Emergency, Severity::Error, Severity::Informational, Severity::Notice, Severity::Warning};
    std::for_each(testSeverities.begin(), testSeverities.end(), [&](Severity sev){
        log.Log(sev, "");
        ASSERT_EQ(standIn.get()->cMsg.severity, sev);});
}

TEST(LoggingBase, LogMessageTest) {
    LoggingBase log;
    auto standIn = std::make_shared<BaseLogHandlerStandIn>();
    log.AddLogHandler(standIn);
    std::string baseStr("abcdefghijklmnopqrstuvwxyz\n"), tmpStr;
    
    for (int i = 0; i < 100; i++) {
        tmpStr += baseStr;
        log.Log(Severity::Critical, tmpStr);
        ASSERT_EQ(tmpStr, standIn.get()->cMsg.message);
    }
}

TEST(LoggingBase, MachineInfoTest) {
    LoggingBase log;
    auto standIn = std::make_shared<BaseLogHandlerStandIn>();
    log.AddLogHandler(standIn);
    log.Log(Severity::Critical, "No message");
    LogMessage msg = standIn.get()->cMsg;
    ASSERT_EQ(msg.host, boost::asio::ip::host_name()) << "Incorrect host name.";
    std::ostringstream ss;
    ss << std::this_thread::get_id();
    ASSERT_EQ(msg.threadId, ss.str()) << "Incorrect thread id.";
    ASSERT_EQ(msg.processId, getpid()) << "Incorrect process id.";
}

TEST(LoggingBase, TimestampTest) {
    LoggingBase log;
    auto standIn = std::make_shared<BaseLogHandlerStandIn>();
    log.AddLogHandler(standIn);
    log.Log(Severity::Critical, "No message");
    LogMessage msg = standIn.get()->cMsg;
    std::chrono::duration<double> time_diff = std::chrono::system_clock::now() - msg.timestamp;
    ASSERT_NEAR(time_diff.count(), 0.0, 0.1) << "Time stamp is incorrect.";
}
