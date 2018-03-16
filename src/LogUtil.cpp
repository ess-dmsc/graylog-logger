//
//  LogUtil.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-19.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include "graylog_logger/LogUtil.hpp"
#include <array>
#include <ciso646>
#include <ctime>
#include <iomanip>

BaseLogHandler::BaseLogHandler(const size_t maxQueueLength)
    : queueLength(maxQueueLength), MessageParser(nullptr) {}

void BaseLogHandler::SetMessageStringCreatorFunction(
    std::function<std::string(const LogMessage &)> ParserFunction) {
  BaseLogHandler::MessageParser = std::move(ParserFunction);
}

void BaseLogHandler::AddMessage(const LogMessage &msg) {
  if (logMessages.size() < queueLength) {
    logMessages.push(msg);
  }
}

bool BaseLogHandler::MessagesQueued() { return logMessages.size() > 0; }

size_t BaseLogHandler::QueueSize() { return logMessages.size(); }

std::string BaseLogHandler::MsgStringCreator(const LogMessage &msg) {
  if (nullptr != MessageParser) {
    return MessageParser(msg);
  }
  std::time_t cTime = std::chrono::system_clock::to_time_t(msg.timestamp);
  char timeBuffer[50];
  size_t bytes = std::strftime(timeBuffer, 50, "%F %T", std::localtime(&cTime));
  std::array<std::string, 8> sevToStr = {{"EMERGENCY", "ALERT", "CRITICAL",
                                          "ERROR", "WARNING", "Notice", "Info",
                                          "Debug"}};
  return std::string(timeBuffer, bytes) + std::string(" (") + msg.host +
         std::string(") ") + sevToStr.at(int(msg.severity)) +
         std::string(": ") + msg.message;
}
