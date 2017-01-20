//
//  ConsoleInterface.hpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-21.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#pragma once

#include <thread>
#include "graylog_logger/LogUtil.hpp"

class ConsoleInterface : public BaseLogHandler {
public:
    ConsoleInterface();
    ~ConsoleInterface();
protected:
    void ThreadFunction();
    std::thread consoleThread;
};
