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
  explicit FileInterface(std::string Name, const size_t MaxQueueLength = 100);
  ~FileInterface();

protected:
  void exitThread();
  std::string FileName;
  void threadFunction();
  std::thread FileThread;
};

} // namespace Log
