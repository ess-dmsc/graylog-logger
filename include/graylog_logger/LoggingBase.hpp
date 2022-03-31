/* Copyright (C) 2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Base logging class for the logging library.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "graylog_logger/LibConfig.hpp"
#include "graylog_logger/LogUtil.hpp"
#include "graylog_logger/ThreadedExecutor.hpp"
#include <sstream>
#include <string>
#include <vector>
#ifdef WITH_FMT
#include <fmt/format.h>
#include <tuple>
#endif
#include "graylog_logger/MinimalApply.hpp"
#include <ciso646>
#include <future>
#include <thread>

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
    if (int(Level) > int(MinSeverity.load(std::memory_order_relaxed))) {
      return;
    }
    auto ThreadId = std::this_thread::get_id();
    Executor.SendWork([=]() {
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
    log(Level, Message,
        std::vector<std::pair<std::string, AdditionalField>>{
            ExtraField,
        });
  }

#ifdef WITH_FMT
  template <typename... Args>
  void fmt_log(const Severity Level, std::string Format, Args... args) {
    if (int(Level) > int(MinSeverity.load(std::memory_order_relaxed))) {
      return;
    }
    auto ThreadId = std::this_thread::get_id();
    auto UsedArguments = std::make_tuple(args...);
    Executor.SendWork([=]() {
      LogMessage cMsg(BaseMsg);
      cMsg.SeverityLevel = Level;
      cMsg.Timestamp = std::chrono::system_clock::now();
      auto format_message = [&Format, &cMsg](const auto &...args) {
        try {
          return fmt::format(Format, args...);
        } catch (fmt::format_error &e) {
          cMsg.SeverityLevel = Log::Severity::Error;
          return fmt::format("graylog-logger internal error. Unable to format "
                             "the string \"{}\". The error was: \"{}\".",
                             Format, e.what());
        }
      };
      cMsg.MessageString = minimal::apply(format_message, UsedArguments);
      std::ostringstream ss;
      ss << ThreadId;
      cMsg.ThreadId = ss.str();
      for (auto &ptr : Handlers) {
        ptr->addMessage(cMsg);
      }
    });
  }
#endif

  virtual void addLogHandler(const LogHandler_P &Handler);

  template <typename valueType>
  void addField(std::string Key, const valueType &Value) {
    Executor.SendWork([=]() { BaseMsg.addField(Key, Value); });
  };
  virtual void removeAllHandlers();
  virtual void setMinSeverity(Severity Level);
  virtual std::vector<LogHandler_P> getHandlers();

  virtual bool flush(std::chrono::system_clock::duration TimeOut) {
    auto FlushCompleted = std::make_shared<std::promise<bool>>();
    auto FlushCompletedValue = FlushCompleted->get_future();
    Executor.SendWork([=, FlushCompleted{std::move(FlushCompleted)}]() {
      std::vector<std::future<bool>> FlushResults;
      for (auto &CHandler : Handlers) {
        FlushResults.push_back(std::async(
            std::launch::async, [=]() { return CHandler->flush(TimeOut); }));
      }
      bool ReturnValue{true};
      for (auto &CFlushResult : FlushResults) {
        CFlushResult.wait();
        if (not CFlushResult.get()) {
          ReturnValue = false;
        }
      }
      FlushCompleted->set_value(ReturnValue);
    });
    std::vector<std::future<bool>> FlushResult;
    FlushCompletedValue.wait();
    return FlushCompletedValue.get();
  }

protected:
  std::atomic<Severity> MinSeverity{Severity::Notice};
  std::vector<LogHandler_P> Handlers;
  LogMessage BaseMsg;
  ThreadedExecutor Executor;
};

} // namespace Log
