/* Copyright (C) 2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Implements utility code for the graylog-logger library.
///
//===----------------------------------------------------------------------===//

#include "graylog_logger/LogUtil.hpp"
#include <array>
#include <ciso646>
#include <ctime>
#include <iomanip>

namespace Log {

BaseLogHandler::BaseLogHandler(const size_t MaxQueueLength)
    : QueueLength(MaxQueueLength) {}

void BaseLogHandler::setMessageStringCreatorFunction(
    std::function<std::string(const LogMessage &)> ParserFunction) {
  BaseLogHandler::MessageParser = std::move(ParserFunction);
}

void BaseLogHandler::addMessage(const LogMessage &Message) {
  if (MessageQueue.size() < QueueLength) {
    MessageQueue.push(Message);
  }
}

bool BaseLogHandler::emptyQueue() { return MessageQueue.empty(); }

size_t BaseLogHandler::queueSize() { return MessageQueue.size(); }

std::string BaseLogHandler::messageToString(const LogMessage &Message) {
  if (nullptr != MessageParser) {
    return MessageParser(Message);
  }
  std::time_t cTime = std::chrono::system_clock::to_time_t(Message.Timestamp);
  char timeBuffer[50];
  size_t bytes = std::strftime(static_cast<char *>(timeBuffer), 50, "%F %T",
                               std::localtime(&cTime));
  std::array<std::string, 8> sevToStr = {{"EMERGENCY", "ALERT", "CRITICAL",
                                          "ERROR", "WARNING", "Notice", "Info",
                                          "Debug"}};
  return std::string(static_cast<char *>(timeBuffer), bytes) +
         std::string(" (") + Message.Host + std::string(") ") +
         sevToStr.at(int(Message.SeverityLevel)) + std::string(": ") +
         Message.MessageString;
}

} // namespace Log
