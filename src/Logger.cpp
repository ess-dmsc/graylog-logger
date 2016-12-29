//
//  Logger.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-19.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include "Logger.hpp"
#include "ConsoleInterface.hpp"
#include "FileInterface.hpp"
#include "GraylogInterface.hpp"


Logger& Logger::Inst() {
    static Logger inst;
    return inst;
}

Logger::Logger() : LoggingBase() {
    LogHandler_P ptr1(new ConsoleInterface());
    AddLogHandler(ptr1);
    
    std::string fileName = "messages.log";
    LogHandler_P ptr2(new FileInterface(fileName));
    AddLogHandler(ptr2);
    
    LogHandler_P ptr3(new GraylogInterface("localhost", 12201));
    AddLogHandler(ptr3);
}

Logger::~Logger() {
    
}

void Logger::AddLogHandler(const LogHandler_P handler) {
    std::lock_guard<std::mutex> guard(vectorMutex);
    bool replaced = false;
    if (dynamic_cast<ConsoleInterface*>(handler.get()) != nullptr) {
        std::for_each(handlers.begin(), handlers.end(), [&](LogHandler_P ptr) {
            if (dynamic_cast<ConsoleInterface*>(ptr.get()) != nullptr) {
                ptr = handler;
                replaced = true;
            }
        });
        if (not replaced) {
            handlers.push_back(handler);
        }
    } else {
        handlers.push_back(handler);
    }
}
