//
//  ConsoleInterface.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-21.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include <iostream>
#include <map>
#include "graylog_logger/ConsoleInterface.hpp"

std::string ConsoleStringCreator(LogMessage &msg) {
    static std::map<int,std::string> sevToStr = {{0,"EMERGENCY"}, {1,"ALERT"}, {2,"CRITICAL"}, {3,"ERROR"}, {4,"WARNING"}, {5,"Notice"}, {6,"Info"}, {7,"Debug"}};
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
