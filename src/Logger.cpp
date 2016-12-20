//
//  Logger.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-19.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include "Logger.hpp"
#include "FileInterface.hpp"

Logger *Logger::lgr = nullptr;

Logger* Logger::Inst() {
    if (nullptr == lgr) {
        Logger::Init("messages.log");
    }
    return lgr;
}

void Logger::Close() {
    if (nullptr == lgr) {
        lgr->Closer();
    }
}

void Logger::Closer() {
    delete this;
}

void Logger::Init(std::string fileName) {
    if (fileName.size() > 0) {
        
    }
}
