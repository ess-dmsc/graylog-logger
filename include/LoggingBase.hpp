//
//  LoggingBase.hpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-19.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#pragma once

#include <string>
#include "ConcurrentQueue.hpp"
#include <ctime>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <algorithm>
#include <thread>
#include <sstream>
#include <mutex>
#include "LogUtil.hpp"

class LoggingBase {
public:
    LoggingBase();
    ~LoggingBase();
    void Log(Severity sev, std::string &message);
    void AddLogHandler(LogHandler_P handler);
    void RemoveAllHandlers();
    std::vector<LogHandler_P> GetHandlers();
protected:
    std::mutex vectorMutex;
    std::vector<LogHandler_P> handlers;
    LogMessage baseMsg;
};
