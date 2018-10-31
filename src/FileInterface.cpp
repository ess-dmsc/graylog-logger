/* Copyright (C) 2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Implements the file writing module.
///
//===----------------------------------------------------------------------===//

#include "graylog_logger/FileInterface.hpp"
#include <ciso646>
#include <fstream>

namespace Log {

FileInterface::FileInterface(std::string Name, const size_t MaxQueueLength)
    : BaseLogHandler(MaxQueueLength), FileName(std::move(Name)),
      FileThread(&FileInterface::threadFunction, this) {}

FileInterface::~FileInterface() { exitThread(); }

void FileInterface::exitThread() {
  LogMessage ExitMsg;
  ExitMsg.MessageString = "exit";
  ExitMsg.ProcessId = -1;
  addMessage(ExitMsg);
  if (FileThread.joinable()) {
    FileThread.join();
  }
}

void FileInterface::threadFunction() {
  std::ofstream outStream(FileName, std::ios_base::app);
  LogMessage TmpMsg;
  while (true) {
    MessageQueue.wait_and_pop(TmpMsg);
    if (std::string("exit") == TmpMsg.MessageString and
        -1 == TmpMsg.ProcessId) {
      break;
    }
    if (outStream.good() and outStream.is_open()) {
      outStream << messageToString(TmpMsg) << std::endl;
    }
  }
}

} // namespace Log
