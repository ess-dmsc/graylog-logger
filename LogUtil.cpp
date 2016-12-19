//
//  LogUtil.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-19.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include "LogUtil.hpp"

BaseLogHandler::BaseLogHandler() : MsgParser(NULL) {
    
}

void BaseHandler::SetMessageStringCreatorFunction(std::string (*MsgParser)(LogMessage &msg)) {
    BaseLogHandler::msgParser = MsgParser;
}

std::string BaseHandler::MsgStringCreator(LogMessage &msg) {
    static std::map<Severity,std::string> sevToStr = {Severity::Emergency:"EMERGENCY", Severity::Alert:"ALERT", Severity::Critical:"CRITICAL", Severity::Error:"ERROR", Severity::Warning:"WARNING", Severity::Notice:"Notice", Severity::Info:"Info", Severity::Debug:"Debug"};
    
}
