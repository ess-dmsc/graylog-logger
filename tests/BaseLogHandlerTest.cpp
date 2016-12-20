//
//  BaseLogHandlerTest.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-20.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//


#include <gtest/gtest.h>
#include <boost/regex.hpp>
#include <iostream>
#include "BaseLogHandlerStandIn.hpp"

const std::string testString("Some test string");

std::string MyStringCreator(LogMessage &msg) {
    return testString;
}

TEST(BaseLogHandler, DefaultStringCreatorTest) {
    LogMessage msg;
    msg.timestamp = std::chrono::system_clock::now();
    msg.message = testString;
    msg.host = "Nohost";
    msg.severity = Severity::Alert;
    BaseLogHandlerStandIn standIn;
    std::string logString = standIn.MsgStringCreator(msg);
    boost::regex exp("\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2} \\(Nohost\\) ALERT: Some test string");
    ASSERT_TRUE(boost::regex_match(logString.c_str(),exp));
}

TEST(BaseLogHandler, SetStringCreatorTest) {
    LogMessage msg;
    BaseLogHandlerStandIn standIn;
    ASSERT_EQ(standIn.msgParser, nullptr);
    standIn.SetMessageStringCreatorFunction(&MyStringCreator);
    ASSERT_EQ(standIn.msgParser, &MyStringCreator);
    ASSERT_EQ(standIn.MsgStringCreator(msg), testString);
    
}
