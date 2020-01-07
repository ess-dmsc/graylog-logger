/* Copyright (C) 2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Implementation file for the interface of the graylog-logger library.
///
//===----------------------------------------------------------------------===//

#include "graylog_logger/Log.hpp"
#include "graylog_logger/Logger.hpp"
#include <ciso646>

namespace Log {
void Msg(const Severity Level, const std::string &Message) {
  Logger::Inst().log(Level, Message);
}

void Msg(const int Level, const std::string &Message) {
  Logger::Inst().log(Severity(Level), Message);
}

void Msg(const Severity Level, const std::string &Message,
         const std::pair<std::string, AdditionalField> &ExtraField) {
  Logger::Inst().log(Level, Message, ExtraField);
}

void Msg(const int Level, const std::string &Message,
         const std::pair<std::string, AdditionalField> &ExtraField) {
  Logger::Inst().log(Severity(Level), Message, ExtraField);
}

void Msg(
    const Severity Level, const std::string &Message,
    const std::vector<std::pair<std::string, AdditionalField>> &ExtraFields) {
  Logger::Inst().log(Level, Message, ExtraFields);
}

void Msg(
    const int Level, const std::string &Message,
    const std::vector<std::pair<std::string, AdditionalField>> &ExtraFields) {
  Logger::Inst().log(Severity(Level), Message, ExtraFields);
}

bool Flush(std::chrono::system_clock::duration Timeout){

}

void AddField(const std::string &Key, const AdditionalField &Value) {
  Logger::Inst().addField(Key, Value);
}

void SetMinimumSeverity(const Severity Level) {
  Logger::Inst().setMinSeverity(Level);
}

void AddLogHandler(const LogHandler_P &Handler) {
  Logger::Inst().addLogHandler(Handler);
}

void RemoveAllHandlers() { Logger::Inst().removeAllHandlers(); }

std::vector<LogHandler_P> GetHandlers() { return Logger::Inst().getHandlers(); }
} // namespace Log
