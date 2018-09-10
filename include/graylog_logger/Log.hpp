/* Copyright (C) 2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Header file with declarations of the functions used to interface to
/// the library.
///
//===----------------------------------------------------------------------===//


#pragma once

#include "graylog_logger/LogUtil.hpp"
#include <vector>

namespace Log {
void Msg(const Severity sev, const std::string &message);
void Msg(const int sev, const std::string &message);
void Msg(const Severity sev, const std::string &message,
         const std::pair<std::string, AdditionalField> &extraField);
void Msg(const int sev, const std::string &message,
         const std::pair<std::string, AdditionalField> &extraField);
void Msg(
    const Severity sev, const std::string &message,
    const std::vector<std::pair<std::string, AdditionalField>> &extraFields);
void Msg(
    const int sev, const std::string &message,
    const std::vector<std::pair<std::string, AdditionalField>> &extraFields);
void AddField(const std::string &key, const AdditionalField &value);
void SetMinimumSeverity(const Severity sev);
void AddLogHandler(const LogHandler_P &handler);
template <typename T>
typename std::enable_if<
    std::is_base_of<BaseLogHandler, T>::value /*, some_type*/>::type
AddLogHandler(T *ptr) {
  AddLogHandler(LogHandler_P(dynamic_cast<BaseLogHandler *>(ptr)));
}
void AddLogHandler(const BaseLogHandler *handler);
void RemoveAllHandlers();
std::vector<LogHandler_P> GetHandlers();
} // namespace Log
