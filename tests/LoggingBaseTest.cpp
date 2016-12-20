//
//  LoggingBaseTest.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-20.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//


#include <gtest/gtest.h>
#include "LoggingBase.hpp"
#include "LogUtil.hpp"

class BaseLogHandlerStandIn : public BaseLogHandler {
public:
    BaseLogHandlerStandIn() : BaseLogHandler() {};
    void AddMessage(LogMessage &msg) {
        cMsg = msg;
    };
    LogMessage cMsg;
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
