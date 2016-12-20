//
//  LoggingBase.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-19.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include "LoggingBase.hpp"

LoggingBase::LoggingBase() {
    std::lock_guard<std::mutex> guard(vectorMutex);
    const int stringBufferSize = 100;
    char stringBuffer[stringBufferSize];
    int res;
    res = gethostname(stringBuffer, stringBufferSize);
    if (0 == res) {
        baseMsg.host = std::string(stringBuffer);
    }
    baseMsg.processId = getpid();
}

LoggingBase::~LoggingBase() {
    std::lock_guard<std::mutex> guard(vectorMutex);
    std::for_each(handlers.begin(), handlers.end(), [](BaseLogHandler *lg){delete lg;});
}

void LoggingBase::Log(Severity sev, std::string &message) {
    LogMessage cMsg(baseMsg);
    cMsg.message = message;
    cMsg.severity = sev;
    std::ostringstream ss;
    ss << std::this_thread::get_id();
    cMsg.threadId = ss.str();
    std::lock_guard<std::mutex> guard(vectorMutex);
    std::for_each(handlers.begin(), handlers.end(), [&](BaseLogHandler *lg){lg->AddMessage(cMsg);});
}

void LoggingBase::AddLogHandler(BaseLogHandler *handler) {
    std::lock_guard<std::mutex> guard(vectorMutex);
    handlers.push_back(handler);
}

void LoggingBase::RemoveAllHandlers() {
    std::lock_guard<std::mutex> guard(vectorMutex);
    std::for_each(handlers.begin(), handlers.end(), [](BaseLogHandler *lg){delete lg;});
    handlers.clear();
}

std::vector<BaseLogHandler*> LoggingBase::GetHandlers() {
    std::lock_guard<std::mutex> guard(vectorMutex);
    return handlers;
}
