//
//  Log.hpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-19.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#pragma once

#include "LogUtil.hpp"

void InitLoggingSystem();
void Log(Severity sev, std::string message);
void AddLogHandler(BaseLogHandler *handler);
void RemoveAllHandlers();
