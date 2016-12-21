//
//  ConsoleInterface.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-21.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include "ConsoleInterface.hpp"

std::string ConsoleStringCreator(LogMessage &msg) {
    static std::map<Severity,std::string> sevToStr = {{Severity::Emergency,"EMERGENCY"}, {Severity::Alert,"ALERT"}, {Severity::Critical,"CRITICAL"}, {Severity::Error,"ERROR"}, {Severity::Warning,"WARNING"}, {Severity::Notice,"Notice"}, {Severity::Info,"Info"}, {Severity::Debug,"Debug"}};
    return sevToStr[msg.severity] + std::string(": ") + msg.message;
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
