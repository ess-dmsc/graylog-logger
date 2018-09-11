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
void Msg(const Severity sev, const std::string &message) {
  Logger::Inst().Log(sev, message);
}

void Msg(const int sev, const std::string &message) {
  Logger::Inst().Log(Severity(sev), message);
}

void Msg(const Severity sev, const std::string &message,
         const std::pair<std::string, AdditionalField> &extraField) {
  Logger::Inst().Log(sev, message, extraField);
}

void Msg(const int sev, const std::string &message,
         const std::pair<std::string, AdditionalField> &extraField) {
  Logger::Inst().Log(Severity(sev), message, extraField);
}

void Msg(
    const Severity sev, const std::string &message,
    const std::vector<std::pair<std::string, AdditionalField>> &extraFields) {
  Logger::Inst().Log(sev, message, extraFields);
}

void Msg(
    const int sev, const std::string &message,
    const std::vector<std::pair<std::string, AdditionalField>> &extraFields) {
  Logger::Inst().Log(Severity(sev), message, extraFields);
}

void AddField(const std::string &key, const AdditionalField &value) {
  Logger::Inst().AddField(key, value);
}

void SetMinimumSeverity(const Severity sev) {
  Logger::Inst().SetMinSeverity(sev);
}

void AddLogHandler(const LogHandler_P &handler) {
  Logger::Inst().AddLogHandler(handler);
}

void RemoveAllHandlers() { Logger::Inst().RemoveAllHandlers(); }

std::vector<LogHandler_P> GetHandlers() { return Logger::Inst().GetHandlers(); }
} // namespace Log
