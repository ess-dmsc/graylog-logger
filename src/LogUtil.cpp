//
//  LogUtil.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-19.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include "LogUtil.hpp"

BaseLogHandler::BaseLogHandler() : msgParser(nullptr) {
}

void BaseLogHandler::SetMessageStringCreatorFunction(std::string (*MsgParser)(LogMessage &msg)) {
    BaseLogHandler::msgParser = MsgParser;
}

BaseLogHandler::~BaseLogHandler() {
    
}

std::string BaseLogHandler::MsgStringCreator(LogMessage &msg) {
    if (nullptr != msgParser) {
        return (*msgParser)(msg);
    }
    static std::map<Severity,std::string> sevToStr = {{Severity::Emergency,"EMERGENCY"}, {Severity::Alert,"ALERT"}, {Severity::Critical,"CRITICAL"}, {Severity::Error,"ERROR"}, {Severity::Warning,"WARNING"}, {Severity::Notice,"Notice"}, {Severity::Info,"Info"}, {Severity::Debug,"Debug"}};
    std::time_t cTime = std::chrono::system_clock::to_time_t(msg.timestamp);
    //std::stringstream ss;
    //ss << std::put_time(std::localtime(&cTime), "%F %T");
    char timeBuffer[50];
    size_t bytes = std::strftime(timeBuffer, 50, "%F %T", std::localtime(&cTime));
    return std::string(timeBuffer, bytes) + std::string(" (") + msg.host + std::string(") ") + sevToStr[msg.severity] + std::string(": ") + msg.message;
}
