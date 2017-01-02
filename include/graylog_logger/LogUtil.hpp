//
//  LogUtil.hpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-19.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#pragma once

#include <string>
#include <chrono>
#include <vector>

typedef std::chrono::time_point<std::chrono::system_clock> system_time;

enum class Severity : int {
    Emergency = 0,
    Alert = 1,
    Critical = 2,
    Error = 3,
    Warning = 4,
    Notice = 5,
    Informational = 6,
    Debug = 7,
};
    
struct LogMessage {
    std::string message;
    system_time timestamp;
    int processId;
    std::string processName;
    std::string host;
    Severity severity;
    std::string threadId;
};

class BaseLogHandler {
public:
    BaseLogHandler();
    virtual ~BaseLogHandler();
    virtual void AddMessage(LogMessage &msg) = 0;
    void SetMessageStringCreatorFunction(std::string (*MsgParser)(LogMessage &msg));
protected:
    const std::vector<std::string> sevToStr = {"EMERGENCY", "ALERT", "CRITICAL", "ERROR", "WARNING", "Notice", "Info", "Debug"};
    std::string (*msgParser)(LogMessage &msg);
    std::string MsgStringCreator(LogMessage &msg);
};

typedef std::shared_ptr<BaseLogHandler>  LogHandler_P;
