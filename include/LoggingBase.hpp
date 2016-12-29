//
//  LoggingBase.hpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-19.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#pragma once

#include <string>
#include <vector>
#include <mutex>
#include "LogUtil.hpp"

class LoggingBase {
public:
    LoggingBase();
    virtual ~LoggingBase();
    virtual void Log(Severity sev, std::string message);
    virtual void AddLogHandler(const LogHandler_P handler);
    virtual void RemoveAllHandlers();
    virtual void SetMinSeverity(Severity sev);
    virtual std::vector<LogHandler_P> GetHandlers();
protected:
    Severity minSeverity;
    std::mutex vectorMutex;
    std::vector<LogHandler_P> handlers;
    LogMessage baseMsg;
};
