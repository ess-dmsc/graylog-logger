//
//  LogUtil.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-19.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include "LogUtil.hpp"

BaseLogHandler::BaseLogHandler() : msgParser(NULL) {
    
}

void BaseLogHandler::SetMessageStringCreatorFunction(std::string (*MsgParser)(LogMessage &msg)) {
    BaseLogHandler::msgParser = MsgParser;
}

BaseLogHandler::~BaseLogHandler() {
    
}

std::string BaseLogHandler::MsgStringCreator(LogMessage &msg) {
    if (NULL != msgParser) {
        return (*msgParser)(msg);
    }
    static std::map<Severity,std::string> sevToStr = {{Severity::Emergency,"EMERGENCY"}, {Severity::Alert,"ALERT"}, {Severity::Critical,"CRITICAL"}, {Severity::Error,"ERROR"}, {Severity::Warning,"WARNING"}, {Severity::Notice,"Notice"}, {Severity::Info,"Info"}, {Severity::Debug,"Debug"}};
    return std::string("(") + msg.host + std::string(") ") + sevToStr[msg.severity] + std::string(": ") + msg.message;
}
