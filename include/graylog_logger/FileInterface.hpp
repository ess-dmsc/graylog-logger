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
#include "graylog_logger/LogUtil.hpp"

class FileInterface : public BaseLogHandler {
public:
    FileInterface(std::string fileName);
    ~FileInterface();
protected:
    std::string fileName;
    void ThreadFunction();
    std::thread fileThread;
};
