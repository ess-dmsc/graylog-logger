/* Copyright (C) 2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief The implementation of the class that implements the actual logging.
///
//===----------------------------------------------------------------------===//

#include "graylog_logger/Logger.hpp"
#include "graylog_logger/ConsoleInterface.hpp"
#include "graylog_logger/FileInterface.hpp"
#include "graylog_logger/GraylogInterface.hpp"
#include <ciso646>

namespace Log {

Logger &Logger::Inst() {
  static Logger inst;
  return inst;
}

Logger::Logger() {
  LogHandler_P ptr1(new ConsoleInterface());
  Logger::AddLogHandler(ptr1);
}

void Logger::AddLogHandler(const LogHandler_P &handler) {
  std::lock_guard<std::mutex> guard(vectorMutex);
  if (dynamic_cast<ConsoleInterface *>(handler.get()) != nullptr) {
    bool replaced = false;
    for (auto ptr : handlers) {
      if (dynamic_cast<ConsoleInterface *>(ptr.get()) != nullptr) {
        ptr = handler;
        replaced = true;
      }
    }
    if (not replaced) {
      handlers.push_back(handler);
    }
  } else {
    handlers.push_back(handler);
  }
}
  
  } // namespace Log
