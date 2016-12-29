//
//  Log.hpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-19.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#pragma once

#include <vector>
#include "graylog_logger/LogUtil.hpp"

namespace Log {
    void Msg(const Severity sev, const std::string message);
    void SetMinimumSeverity(const Severity sev);
    void AddLogHandler(const LogHandler_P handler);
    void RemoveAllHandlers();
    std::vector<LogHandler_P> GetHandlers();
}
