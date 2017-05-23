//
//  LogUtil.hpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-19.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#pragma once

#include "graylog_logger/ConcurrentQueue.hpp"
#include <chrono>
#include <memory>
#include <string>

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
  AdditionalField() : intVal(0), dblVal(0), FieldType(Type::typeStr) {};
  AdditionalField(double val) : intVal(0), dblVal(val), FieldType(Type::typeDbl) {};
  AdditionalField(const std::string &val) : intVal(0), dblVal(0), strVal(val), FieldType(Type::typeStr) {};
  AdditionalField(std::int64_t val) : intVal(val), dblVal(0), FieldType(Type::typeInt) {};
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
  LogMessage() : severity(Severity::Debug) {};
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
  }
};

class BaseLogHandler {
public:
  BaseLogHandler(const size_t maxQueueLength = 100);
  virtual ~BaseLogHandler();
  virtual void AddMessage(const LogMessage &msg);
  virtual bool MessagesQueued();
  virtual size_t QueueSize();
  void
  SetMessageStringCreatorFunction(std::string (*MsgParser)(LogMessage &msg));

protected:
  size_t queueLength;
  ConcurrentQueue<LogMessage> logMessages;
  std::string (*msgParser)(LogMessage &msg);
  std::string MsgStringCreator(LogMessage &msg);
};

typedef std::shared_ptr<BaseLogHandler> LogHandler_P;
