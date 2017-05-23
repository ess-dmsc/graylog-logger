//
//  FileInterface.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-19.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include "graylog_logger/FileInterface.hpp"
#include <ciso646>
#include <fstream>

FileInterface::FileInterface(const std::string &fileName, const size_t maxQueueLength)
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
