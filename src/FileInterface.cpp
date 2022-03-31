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

FileInterface::FileInterface(std::string const &Name,
                             const size_t MaxQueueLength)
    : BaseLogHandler(), FileStream(Name, std::ios::app) {
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
      FileStream << BaseLogHandler::messageToString(Message) +
                        std::string("\n");
    }
  });
}

void FileInterface::setMessageStringCreatorFunction(
                                                       std::function<std::string(const LogMessage &)> ParserFunction) {
  Executor.SendWork([=]() {
    BaseLogHandler::setMessageStringCreatorFunction(ParserFunction);
  });
}

bool FileInterface::flush(std::chrono::system_clock::duration TimeOut) {
  auto WorkDone = std::make_shared<std::promise<void>>();
  auto WorkDoneFuture = WorkDone->get_future();
  Executor.SendWork([=, WorkDone{std::move(WorkDone)}]() {
    FileStream.flush();
    WorkDone->set_value();
  });
  return std::future_status::ready == WorkDoneFuture.wait_for(TimeOut);
}

bool FileInterface::emptyQueue() { return Executor.size_approx() == 0; }

size_t FileInterface::queueSize() { return Executor.size_approx(); }

} // namespace Log
