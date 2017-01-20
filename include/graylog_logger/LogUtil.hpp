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
#include <memory>
#include "graylog_logger/ConcurrentQueue.hpp"

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
    BaseLogHandler(int maxQueueLength = 100);
    virtual ~BaseLogHandler();
    virtual void AddMessage(const LogMessage &msg);
    virtual bool MessagesQueued();
    virtual size_t QueueSize();
    void SetMessageStringCreatorFunction(std::string (*MsgParser)(LogMessage &msg));
protected:
    int queueLength;
    ConcurrentQueue<LogMessage> logMessages;
    std::string (*msgParser)(LogMessage &msg);
    std::string MsgStringCreator(LogMessage &msg);
};

typedef std::shared_ptr<BaseLogHandler>  LogHandler_P;
