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
    
struct AdditionalField {
    AdditionalField() {
        FieldType = Type::typeStr;
    };
    AdditionalField(double val) {
        FieldType = Type::typeDbl;
        dblVal = val;
    };
    AdditionalField(std::string val) {
        FieldType = Type::typeStr;
        strVal = val;
    };
    AdditionalField(std::int64_t val) {
        FieldType = Type::typeInt;
        intVal = val;
    };
    enum class Type : char {
        typeStr = 0,
        typeDbl = 1,
        typeInt = 2,
    } FieldType;
    std::string strVal;
    std::int64_t intVal;
    double dblVal;
};
    
struct LogMessage {
    std::string message;
    system_time timestamp;
    int processId;
    std::string processName;
    std::string host;
    Severity severity;
    std::string threadId;
    std::vector<std::pair<std::string, AdditionalField>> additionalFields;
    template <typename valueType>
    void AddField(std::string key, const valueType &val) {
        int fieldLoc = -1;
        for (int i = 0; i < additionalFields.size(); i++) {
            if (additionalFields[i].first == key) {
                fieldLoc = i;
                break;
            }
        }
        if (-1 == fieldLoc) {
            additionalFields.push_back({key, val});
        } else {
            additionalFields[fieldLoc] = {key, val};
        }
    };
};

class BaseLogHandler {
public:
    BaseLogHandler(size_t maxQueueLength = 100);
    virtual ~BaseLogHandler();
    virtual void AddMessage(const LogMessage &msg);
    virtual bool MessagesQueued();
    virtual size_t QueueSize();
    void SetMessageStringCreatorFunction(std::string (*MsgParser)(LogMessage &msg));
protected:
    size_t queueLength;
    ConcurrentQueue<LogMessage> logMessages;
    std::string (*msgParser)(LogMessage &msg);
    std::string MsgStringCreator(LogMessage &msg);
};

typedef std::shared_ptr<BaseLogHandler>  LogHandler_P;
