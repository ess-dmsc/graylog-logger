/* Copyright (C) 2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Header file for the logging class.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "graylog_logger/LoggingBase.hpp"

namespace Log {

class Logger : private LoggingBase {
public:
  static Logger &Inst();
  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;
  virtual void AddLogHandler(const LogHandler_P &handler) override;
  using LoggingBase::AddField;
  using LoggingBase::GetHandlers;
  using LoggingBase::Log;
  using LoggingBase::RemoveAllHandlers;
  using LoggingBase::SetMinSeverity;

private:
  Logger();
  ~Logger() = default;
};
  
  } // namespace Log
