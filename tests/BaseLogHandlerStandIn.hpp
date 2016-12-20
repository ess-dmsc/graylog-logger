//
//  BaseLogHandlerStandIn.hpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-20.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#pragma once

#include "LogUtil.hpp"

class BaseLogHandlerStandIn : public BaseLogHandler {
public:
    BaseLogHandlerStandIn() : BaseLogHandler() {};
    void AddMessage(LogMessage &msg) {
        cMsg = msg;
    };
    LogMessage cMsg;
    using BaseLogHandler::MsgStringCreator;
    using BaseLogHandler::msgParser;
};
