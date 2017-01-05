//
//  ConsoleInterface.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-21.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include <iostream>
#include <array>
#include "graylog_logger/ConsoleInterface.hpp"

std::string ConsoleStringCreator(LogMessage &msg) {
    static std::array<std::string, 8> sevToStr = {"EMERGENCY","ALERT","CRITICAL","ERROR","WARNING","Notice","Info","Debug"};
    return sevToStr[int(msg.severity)] + std::string(": ") + msg.message;
}

ConsoleInterface::ConsoleInterface() : BaseLogHandler(), consoleThread(&ConsoleInterface::ThreadFunction, this) {
    BaseLogHandler::SetMessageStringCreatorFunction(ConsoleStringCreator);
}

ConsoleInterface::~ConsoleInterface() {
    LogMessage exitMsg;
    exitMsg.message = "exit";
    exitMsg.processId = -1;
    AddMessage(exitMsg);
    consoleThread.join();
}

void ConsoleInterface::AddMessage(LogMessage &msg) {
    msgQueue.push(msg);
}

void ConsoleInterface::ThreadFunction() {
    LogMessage tmpMsg;
    while (true) {
        msgQueue.wait_and_pop(tmpMsg);
        if (std::string("exit") == tmpMsg.message and -1 ==  tmpMsg.processId) {
            break;
        }
        std::cout << MsgStringCreator(tmpMsg) << std::endl;
    }
}
