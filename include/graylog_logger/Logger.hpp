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
  virtual void addLogHandler(const LogHandler_P &Handler) override;
  using LoggingBase::addField;
  using LoggingBase::flush;
  using LoggingBase::getHandlers;
  using LoggingBase::log;
  using LoggingBase::removeAllHandlers;
  using LoggingBase::setMinSeverity;
#ifdef WITH_FMT
  using LoggingBase::fmt_log;
#endif

protected:
  Logger();
  ~Logger() = default;
};

} // namespace Log
