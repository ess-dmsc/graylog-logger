//
//  LoggingBase.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-19.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//


#include "graylog_logger/LoggingBase.hpp"
#include <sys/types.h>
#include <thread>
#include <sstream>
#include <chrono>
#include <ciso646>

#ifdef _WIN32
#include <Winsock2.h>
#include <process.h>
#define getpid _getpid
#else
#include <unistd.h>
#endif

LoggingBase::LoggingBase() {
    std::lock_guard<std::mutex> guard1(vectorMutex);
    minSeverity = Severity::Notice;
    const int stringBufferSize = 100;
    char stringBuffer[stringBufferSize];
    int res;
    res = gethostname(stringBuffer, stringBufferSize);
    std::lock_guard<std::mutex> guard2(baseMsgMutex);
    if (0 == res) {
        baseMsg.host = std::string(stringBuffer);
    }
    baseMsg.processId = getpid();
}

LoggingBase::~LoggingBase() {
    std::lock_guard<std::mutex> guard(vectorMutex);
    handlers.clear();
}

void LoggingBase::Log(Severity sev, std::string message) {
    if (int(sev) > int(minSeverity)) {
        return;
    }
    baseMsgMutex.lock();
    LogMessage cMsg(baseMsg);
    baseMsgMutex.unlock();
    cMsg.timestamp = std::chrono::system_clock::now();
    cMsg.message = message;
    cMsg.severity = sev;
    std::ostringstream ss;
    ss << std::this_thread::get_id();
    cMsg.threadId = ss.str();
    std::lock_guard<std::mutex> guard(vectorMutex);
    for(auto ptr : handlers) {
        ptr.get()->AddMessage(cMsg);
    }
}

void LoggingBase::Log(Severity sev, std::string message, std::vector<std::pair<std::string, AdditionalField>> extraFields) {
    if (int(sev) > int(minSeverity)) {
        return;
    }
    baseMsgMutex.lock();
    LogMessage cMsg(baseMsg);
    baseMsgMutex.unlock();
    for (auto &fld : extraFields) {
        cMsg.AddField(fld.first, fld.second);
    }
    cMsg.timestamp = std::chrono::system_clock::now();
    cMsg.message = message;
    cMsg.severity = sev;
    std::ostringstream ss;
    ss << std::this_thread::get_id();
    cMsg.threadId = ss.str();
    std::lock_guard<std::mutex> guard(vectorMutex);
    for(auto ptr : handlers) {
        ptr.get()->AddMessage(cMsg);
    }
}

void LoggingBase::AddLogHandler(const LogHandler_P handler) {
    std::lock_guard<std::mutex> guard(vectorMutex);
    handlers.push_back(handler);
}

void LoggingBase::RemoveAllHandlers() {
    std::lock_guard<std::mutex> guard(vectorMutex);
    handlers.clear();
}

std::vector<LogHandler_P> LoggingBase::GetHandlers() {
    std::lock_guard<std::mutex> guard(vectorMutex);
    return handlers;
}

void LoggingBase::SetMinSeverity(Severity sev) {
    std::lock_guard<std::mutex> guard(vectorMutex);
    minSeverity = sev;
}
