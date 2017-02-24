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
#include "graylog_logger/LogUtil.hpp"

class LoggingBase {
public:
    LoggingBase();
    virtual ~LoggingBase();
    virtual void Log(const Severity sev, const std::string &message);
    virtual void Log(const Severity sev, const std::string &message, const std::vector<std::pair<std::string, AdditionalField>> &extraFields);
    virtual void Log(const Severity sev, const std::string &message, const std::pair<std::string, AdditionalField> &extraField);
    virtual void AddLogHandler(const LogHandler_P handler);
    template<typename valueType>
    void AddField(std::string key, const valueType &value) {
        std::lock_guard<std::mutex> guard(baseMsgMutex);
        baseMsg.AddField(key, value);
    };
    virtual void RemoveAllHandlers();
    virtual void SetMinSeverity(Severity sev);
    virtual std::vector<LogHandler_P> GetHandlers();
protected:
    Severity minSeverity;
    std::mutex vectorMutex;
    std::mutex baseMsgMutex;
    std::vector<LogHandler_P> handlers;
    LogMessage baseMsg;
};
