//
//  FileInterface.hpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-19.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#pragma once

#include <string>
#include <thread>
#include <fstream>
#include "ConcurrentQueue.hpp"
#include "LogUtil.hpp"

class FileInterface : public BaseLogHandler {
public:
    FileInterface(std::string fileName);
    ~FileInterface();
    void AddMessage(LogMessage &msg);
protected:
    std::string fileName;
    void ThreadFunction();
    ConcurrentQueue<LogMessage> msgQueue;
    std::thread fileThread;
};
