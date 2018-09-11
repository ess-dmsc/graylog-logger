/* Copyright (C) 2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Header of file logging interface.
///
//===----------------------------------------------------------------------===//

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
