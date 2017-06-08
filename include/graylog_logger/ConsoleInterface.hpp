//
//  ConsoleInterface.hpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-21.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

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
