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

FileInterface::FileInterfacess(const std::string &fileName,
                             const size_t maxQueueLength)
    : BaseLogHandler(maxQueueLength), fName(fileName),
      fileThread(&FileInterface::ThreadFunction, this) {}

FileInterface::~FileInterface() { ExitThread(); }

void FileInterface::ExitThread() {
  LogMessage exitMsg;
  exitMsg.message = "exit";
  exitMsg.processId = -1;
  AddMessage(exitMsg);
  if (fileThread.joinable()) {
    fileThread.join();
  }
}

void FileInterface::ThreadFunction() {
  std::ofstream outStream(fName, std::ios_base::app);
  LogMessage tmpMsg;
  while (true) {
    logMessages.wait_and_pop(tmpMsg);
    if (std::string("exit") == tmpMsg.message and -1 == tmpMsg.processId) {
      break;
    }
    if (outStream.good() and outStream.is_open()) {
      outStream << MsgStringCreator(tmpMsg) << std::endl;
    }
  }
}
