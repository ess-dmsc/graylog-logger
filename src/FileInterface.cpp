/* Copyright (C) 2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Implements the file writing module.
///
//===----------------------------------------------------------------------===//

#include "graylog_logger/FileInterface.hpp"
#include "graylog_logger/Log.hpp"
#include <ciso646>
#include <fstream>

namespace Log {

FileInterface::FileInterface(std::string const &Name, const size_t MaxQueueLength)
    : BaseLogHandler(MaxQueueLength), FileStream(Name, std::ios::app) {
  if (FileStream.is_open() and FileStream.good()) {
    Log::Msg(Severity::Info, "Started logging to log file: \"" + Name + "\"");
  } else {
    Log::Msg(Severity::Error,
             "Unable to open log file for logging: \"" + Name + "\"");
  }
}

void FileInterface::addMessage(const LogMessage &Message) {
  Executor.SendWork([=]() {
    if (FileStream.good() and FileStream.is_open()) {
      FileStream << BaseLogHandler::messageToString(Message) << std::endl;
    }
  });
}

} // namespace Log
