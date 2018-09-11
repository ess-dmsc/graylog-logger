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

std::string ConsoleStringCreator(const LogMessage &msg) {
  std::array<std::string, 8> sevToStr = {{"EMERGENCY", "ALERT", "CRITICAL",
                                          "ERROR", "WARNING", "Notice", "Info",
                                          "Debug"}};
  return sevToStr.at(int(msg.severity)) + std::string(": ") + msg.message;
}

ConsoleInterface::ConsoleInterface(size_t maxQueueLength)
    : BaseLogHandler(maxQueueLength),
      consoleThread(&ConsoleInterface::ThreadFunction, this) {
  BaseLogHandler::SetMessageStringCreatorFunction(ConsoleStringCreator);
}

ConsoleInterface::~ConsoleInterface() { ExitThread(); }

void ConsoleInterface::ExitThread() {
  LogMessage exitMsg;
  exitMsg.message = "exit";
  exitMsg.processId = -1;
  AddMessage(exitMsg);
  if (consoleThread.joinable()) {
    consoleThread.join();
  }
}

void ConsoleInterface::ThreadFunction() {
  LogMessage tmpMsg;
  while (true) {
    logMessages.wait_and_pop(tmpMsg);
    if (std::string("exit") == tmpMsg.message and -1 == tmpMsg.processId) {
      break;
    }
    std::cout << MsgStringCreator(tmpMsg) << std::endl;
  }
}
