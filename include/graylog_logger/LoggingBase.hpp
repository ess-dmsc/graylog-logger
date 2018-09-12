/* Copyright (C) 2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Base logging class for the logging library.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "graylog_logger/LogUtil.hpp"
#include <mutex>
#include <string>
#include <vector>

namespace Log {

class LoggingBase {
public:
  LoggingBase();
  virtual ~LoggingBase();
  virtual void log(const Severity Level, const std::string &Message);
  virtual void
  log(const Severity Level, const std::string &Message,
      const std::vector<std::pair<std::string, AdditionalField>> &ExtraFields);
  virtual void log(const Severity Level, const std::string &Message,
                   const std::pair<std::string, AdditionalField> &ExtraField);
  virtual void addLogHandler(const LogHandler_P &Handler);
  template <typename valueType>
  void addField(std::string Key, const valueType &Value) {
    std::lock_guard<std::mutex> Guard(BaseMsgMutex);
    BaseMsg.AddField(Key, Value);
  };
  virtual void removeAllHandlers();
  virtual void setMinSeverity(Severity Level);
  virtual std::vector<LogHandler_P> getHandlers();

protected:
  Severity MinSeverity;
  std::mutex VectorMutex;
  std::mutex BaseMsgMutex;
  std::vector<LogHandler_P> Handlers;
  LogMessage BaseMsg;
};

} // namespace Log
