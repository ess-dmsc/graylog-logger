/* Copyright (C) 2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Header file console interface.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "graylog_logger/LogUtil.hpp"
#include <thread>

class ConsoleInterface : public BaseLogHandler {
public:
  ConsoleInterface(size_t maxQueueLength = 100);
  ~ConsoleInterface();

protected:
  void ExitThread();
  void ThreadFunction();
  std::thread consoleThread;
};
