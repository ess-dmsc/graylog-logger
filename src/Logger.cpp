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
  Logger::addLogHandler(ptr1);
}

void Logger::addLogHandler(const LogHandler_P &Handler) {
  std::lock_guard<std::mutex> guard(VectorMutex);
  if (dynamic_cast<ConsoleInterface *>(Handler.get()) != nullptr) {
    bool replaced = false;
    for (auto ptr : Handlers) {
      if (dynamic_cast<ConsoleInterface *>(ptr.get()) != nullptr) {
        ptr = Handler;
        replaced = true;
      }
    }
    if (not replaced) {
      Handlers.push_back(Handler);
    }
  } else {
    Handlers.push_back(Handler);
  }
}

} // namespace Log
