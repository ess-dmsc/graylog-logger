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

void BaseLogHandler::setMessageStringCreatorFunction(
    std::function<std::string(const LogMessage &)> ParserFunction) {
  BaseLogHandler::MessageParser = std::move(ParserFunction);
}

std::string BaseLogHandler::messageToString(const LogMessage &Message) {
  if (nullptr != MessageParser) {
    return MessageParser(Message);
  }
  std::time_t cTime = std::chrono::system_clock::to_time_t(Message.Timestamp);
  const size_t TimeBufferSize{50};
  std::array<char, TimeBufferSize> TimeBuffer{};
  size_t BytesWritten =
      std::strftime(static_cast<char *>(TimeBuffer.data()), TimeBufferSize,
                    "%F %T", std::localtime(&cTime));
  std::array<std::string, 8> sevToStr = {{"EMERGENCY", "ALERT", "CRITICAL",
                                          "ERROR", "WARNING", "Notice", "Info",
                                          "Debug"}};
  return std::string(static_cast<char *>(TimeBuffer.data()), BytesWritten) +
         std::string(" (") + Message.Host + std::string(") ") +
         sevToStr.at(int(Message.SeverityLevel)) + std::string(": ") +
         Message.MessageString;
}

} // namespace Log
