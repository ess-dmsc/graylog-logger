/* Copyright (C) 2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Implementation file for the console logging class.
///
//===----------------------------------------------------------------------===//

#include "graylog_logger/ConsoleInterface.hpp"
#include <array>
#include <ciso646>
#include <iostream>

namespace Log {

std::string ConsoleStringCreator(const LogMessage &Message) {
  std::array<std::string, 8> sevToStr = {{"EMERGENCY", "ALERT", "CRITICAL",
                                          "ERROR", "WARNING", "Notice", "Info",
                                          "Debug"}};
  return sevToStr.at(int(Message.SeverityLevel)) + std::string(": ") +
         Message.MessageString;
}

ConsoleInterface::ConsoleInterface(size_t MaxQueueLength)
    : BaseLogHandler() {
  BaseLogHandler::setMessageStringCreatorFunction(ConsoleStringCreator);
}
void ConsoleInterface::addMessage(const LogMessage &Message) {
  Executor.SendWork(
      [=]() { std::cout << BaseLogHandler::MessageParser(Message) << "\n"; });
}

} // namespace Log
