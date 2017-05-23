//
//  FileInterface.hpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-19.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#pragma once

#include "graylog_logger/LogUtil.hpp"
#include <string>
#include <thread>

class FileInterface : public BaseLogHandler {
public:
  FileInterface(const std::string &fileName, const size_t maxQueueLength = 100);
  ~FileInterface();

protected:
  void ExitThread();
  std::string fName;
  void ThreadFunction();
  std::thread fileThread;
};
