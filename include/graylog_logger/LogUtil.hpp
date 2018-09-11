/* Copyright (C) 2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Contains basic structures and classes required by the graylog-logger
/// library.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "graylog_logger/ConcurrentQueue.hpp"
#include <chrono>
#include <functional>
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
  Info = 6,
  Debug = 7,
};

struct AdditionalField {
  AdditionalField() : FieldType(Type::typeStr), intVal(0), dblVal(0){};
  AdditionalField(double val)
      : FieldType(Type::typeDbl), intVal(0), dblVal(val){};
  AdditionalField(const std::string &val)
      : FieldType(Type::typeStr), strVal(val), intVal(0), dblVal(0){};
  AdditionalField(std::int64_t val)
      : FieldType(Type::typeInt), intVal(val), dblVal(0){};
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
  LogMessage() : severity(Severity::Debug){};
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
    for (size_t i = 0; i < additionalFields.size(); i++) {
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
  virtual ~BaseLogHandler() = default;
  virtual void AddMessage(const LogMessage &msg);
  virtual bool MessagesQueued();
  virtual size_t QueueSize();
  void SetMessageStringCreatorFunction(
      std::function<std::string(const LogMessage &)> ParserFunction);

protected:
  size_t queueLength;
  ConcurrentQueue<LogMessage> logMessages;
  std::function<std::string(const LogMessage &)> MessageParser;
  std::string MsgStringCreator(const LogMessage &msg);
};

typedef std::shared_ptr<BaseLogHandler> LogHandler_P;
