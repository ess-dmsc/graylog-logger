//
//  ConsoleInterface.hpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-21.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#pragma once

#include <iostream>
#include <thread>
#include "ConcurrentQueue.hpp"
#include "LogUtil.hpp"

class ConsoleInterface : public BaseLogHandler {
public:
    ConsoleInterface();
    ~ConsoleInterface();
    void AddMessage(LogMessage &msg);
protected:
    void ThreadFunction();
    ConcurrentQueue<LogMessage> msgQueue;
    std::thread consoleThread;
};
