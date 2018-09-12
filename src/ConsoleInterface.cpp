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
  return sevToStr.at(int(Message.SeverityLevel)) + std::string(": ") + Message.MessageString;
}

ConsoleInterface::ConsoleInterface(size_t maxQueueLength)
    : BaseLogHandler(maxQueueLength),
      consoleThread(&ConsoleInterface::ThreadFunction, this) {
  BaseLogHandler::setMessageStringCreatorFunction(ConsoleStringCreator);
}

ConsoleInterface::~ConsoleInterface() { ExitThread(); }

void ConsoleInterface::ExitThread() {
  LogMessage ExitMsg;
  ExitMsg.MessageString = "exit";
  ExitMsg.ProcessId = -1;
  addMessage(ExitMsg);
  if (consoleThread.joinable()) {
    consoleThread.join();
  }
}

void ConsoleInterface::ThreadFunction() {
  LogMessage tmpMsg;
  while (true) {
    MessageQueue.wait_and_pop(tmpMsg);
    if (std::string("exit") == tmpMsg.MessageString and -1 == tmpMsg.ProcessId) {
      break;
    }
    std::cout << messageToString(tmpMsg) << std::endl;
  }
}
  
  } // namespace Log
