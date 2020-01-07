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
#include <string>
#include <vector>
#include "graylog_logger/ThreadedExecutor.hpp"
#include <sstream>

namespace Log {

class LoggingBase {
public:
  LoggingBase();
  virtual ~LoggingBase();
  virtual void log(const Severity Level, const std::string &Message) {
    log(Level, Message, std::vector<std::pair<std::string, AdditionalField>>());
  }
  virtual void
  log(const Severity Level, const std::string &Message,
      const std::vector<std::pair<std::string, AdditionalField>> &ExtraFields) {
    auto ThreadId = std::this_thread::get_id();
    Executor.SendWork([=](){
      if (int(Level) > int(MinSeverity)) {
        return;
      }
      LogMessage cMsg(BaseMsg);
      for (auto &fld : ExtraFields) {
        cMsg.addField(fld.first, fld.second);
      }
      cMsg.Timestamp = std::chrono::system_clock::now();
      cMsg.MessageString = Message;
      cMsg.SeverityLevel = Level;
      std::ostringstream ss;
      ss << ThreadId;
      cMsg.ThreadId = ss.str();
      for (auto &ptr : Handlers) {
        ptr->addMessage(cMsg);
      }
    });
  }
  virtual void log(const Severity Level, const std::string &Message,
                   const std::pair<std::string, AdditionalField> &ExtraField) {
    log(Level, Message, std::vector<std::pair<std::string, AdditionalField>>{
        ExtraField,
    });
  }
  virtual void addLogHandler(const LogHandler_P Handler);

  template <typename valueType>
  void addField(std::string Key, const valueType &Value) {
    Executor.SendWork([=](){
      BaseMsg.addField(Key, Value);
    });
  };
  virtual void removeAllHandlers();
  virtual void setMinSeverity(Severity Level);
  virtual std::vector<LogHandler_P> getHandlers();

protected:
  Severity MinSeverity{Severity::Notice};
  std::mutex BaseMsgMutex;
  std::vector<LogHandler_P> Handlers;
  LogMessage BaseMsg;
  ThreadedExecutor Executor;
};

} // namespace Log
