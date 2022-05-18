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
  std::array<std::string, 9> sevToStr = {{"EMERGENCY", "ALERT", "CRITICAL",
                                          "ERROR", "WARNING", "Notice", "Info",
                                          "Debug", "Trace"}};
  return sevToStr.at(int(Message.SeverityLevel)) + std::string(": ") +
         Message.MessageString;
}

ConsoleInterface::ConsoleInterface() {
  BaseLogHandler::setMessageStringCreatorFunction(ConsoleStringCreator);
}

using std::string_literals::operator""s;
void ConsoleInterface::addMessage(const LogMessage &Message) {
  Executor.SendWork([=]() {
    printf(
        "%s",
        (BaseLogHandler::MessageParser(Message) + std::string("\n")).c_str());
  });
}

void ConsoleInterface::setMessageStringCreatorFunction(
    std::function<std::string(const LogMessage &)> ParserFunction) {
  Executor.SendWork([=]() {
    BaseLogHandler::setMessageStringCreatorFunction(ParserFunction);
  });
}

bool ConsoleInterface::flush(std::chrono::system_clock::duration TimeOut) {
  auto WorkDone = std::make_shared<std::promise<void>>();
  auto WorkDoneFuture = WorkDone->get_future();
  Executor.SendWork([=, WorkDone{std::move(WorkDone)}]() {
    std::cout.flush();
    WorkDone->set_value();
  });
  return std::future_status::ready == WorkDoneFuture.wait_for(TimeOut);
}

bool ConsoleInterface::emptyQueue() { return Executor.size_approx() == 0; }

size_t ConsoleInterface::queueSize() { return Executor.size_approx(); }

} // namespace Log
