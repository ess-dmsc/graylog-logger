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
#include <string>
#include <thread>

namespace Log {

class FileInterface : public BaseLogHandler {
public:
  FileInterface(std::string FileName, const size_t maxQueueLength = 100);
  ~FileInterface();

protected:
  void ExitThread();
  std::string FileName;
  void ThreadFunction();
  std::thread fileThread;
};

} // namespace Log
