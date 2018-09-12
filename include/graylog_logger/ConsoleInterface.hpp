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
#include <thread>

namespace Log {

class ConsoleInterface : public BaseLogHandler {
public:
  ConsoleInterface(size_t MaxQueueLength = 100);
  ~ConsoleInterface();

protected:
  void exitThread();
  void threadFunction();
  std::thread ConsoleThread;
};

} // namespace Log
