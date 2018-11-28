/* Copyright (C) 2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Header file of console interface.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "graylog_logger/LogUtil.hpp"
#include "graylog_logger/ThreadedExecutor.hpp"

namespace Log {

class ConsoleInterface : public BaseLogHandler {
public:
  explicit ConsoleInterface(size_t MaxQueueLength = 100);
  void addMessage(const LogMessage &Message) override;

protected:
  ThreadedExecutor Executor; // Must be last
};

} // namespace Log
