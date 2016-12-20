//
//  FileInterface.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-19.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include "FileInterface.hpp"

FileInterface::FileInterface(std::string fileName) : BaseLogHandler(), fileName(fileName), fileThread(&FileInterface::ThreadFunction, this) {
    
}

void FileInterface::AddMessage(LogMessage msg) {
    msgQueue.push(msg);
}

FileInterface::~FileInterface() {
    LogMessage exitMsg;
    exitMsg.message = "exit";
    exitMsg.processId = -1;
    AddMessage(exitMsg);
    fileThread.join();
}

void FileInterface::ThreadFunction() {
    std::ofstream outStream(fileName);
    LogMessage tmpMsg;
    while (true) {
        msgQueue.wait_and_pop(tmpMsg);
        if (std::string("exit") == tmpMsg.message and -1 ==  tmpMsg.processId) {
            break;
        }
        if (outStream.good() and outStream.is_open()) {
            outStream << MsgStringCreator(tmpMsg) << std::endl;
            
        }
    }
}
