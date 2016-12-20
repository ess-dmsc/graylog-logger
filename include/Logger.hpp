//
//  Logger.hpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-19.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#pragma once

#include "LoggingBase.hpp"
#include <string>

class Logger : private LoggingBase {
public:
    static void Init(std::string fileName);
    static void Close();
    static Logger* Inst();
    
private:
    void Closer();
    static Logger *lgr;
};
