//
//  ConsoleInterface.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-21.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include <ciso646>
#include <iostream>
#include <array>
#include "graylog_logger/ConsoleInterface.hpp"

std::string ConsoleStringCreator(LogMessage &msg) {
    static std::array<std::string, 8> sevToStr = {"EMERGENCY","ALERT","CRITICAL","ERROR","WARNING","Notice","Info","Debug"};
    return sevToStr[int(msg.severity)] + std::string(": ") + msg.message;
}

ConsoleInterface::ConsoleInterface(int maxQueueLength) : BaseLogHandler(maxQueueLength), consoleThread(&ConsoleInterface::ThreadFunction, this) {
    BaseLogHandler::SetMessageStringCreatorFunction(ConsoleStringCreator);
}

ConsoleInterface::~ConsoleInterface() {
    ExitThread();
}

void ConsoleInterface::ExitThread() {
    LogMessage exitMsg;
    exitMsg.message = "exit";
    exitMsg.processId = -1;
    AddMessage(exitMsg);
    if (consoleThread.joinable()){
        consoleThread.join();
    }
}

void ConsoleInterface::ThreadFunction() {
    LogMessage tmpMsg;
    while (true) {
        logMessages.wait_and_pop(tmpMsg);
        if (std::string("exit") == tmpMsg.message and -1 ==  tmpMsg.processId) {
            break;
        }
        std::cout << MsgStringCreator(tmpMsg) << std::endl;
    }
}
