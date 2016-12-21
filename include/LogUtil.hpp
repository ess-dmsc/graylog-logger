//
//  LogUtil.hpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-19.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#pragma once

#include <string>
#include <map>
#include <memory>
#include <chrono>
#include <ctime>
#include <iomanip>
//#include <sstream>

typedef std::chrono::time_point<std::chrono::system_clock> system_time;

enum class Severity {Emergency, Alert, Critical, Error, Warning, Notice, Info, Debug};
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
    std::string (*msgParser)(LogMessage &msg);
    std::string MsgStringCreator(LogMessage &msg);
};

typedef std::shared_ptr<BaseLogHandler>  LogHandler_P;
