/* Copyright (C) 2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Header file of logging interface.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "graylog_logger/LogUtil.hpp"
#include "graylog_logger/ThreadedExecutor.hpp"
#include <fstream>
#include <string>

namespace Log {

class FileInterface : public BaseLogHandler {
public:
  explicit FileInterface(std::string Name, const size_t MaxQueueLength = 100);
  void addMessage(const LogMessage &Message) override;

protected:
  std::ofstream FileStream;
  ThreadedExecutor Executor; // Must be last
};

} // namespace Log
