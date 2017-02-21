//
//  Log.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-19.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include <ciso646>
#include "graylog_logger/Log.hpp"
#include "graylog_logger/Logger.hpp"

namespace Log {
    void Msg(const Severity sev, const std::string &message) {
        Logger::Inst().Log(sev, message);
    }
    
    void Msg(const int sev, const std::string &message) {
        Logger::Inst().Log(Severity(sev), message);
    }
    
    void SetMinimumSeverity(const Severity sev) {
        Logger::Inst().SetMinSeverity(sev);
    }
    
    void AddLogHandler(const LogHandler_P handler) {
        Logger::Inst().AddLogHandler(handler);
    }
    
    void RemoveAllHandlers() {
        Logger::Inst().RemoveAllHandlers();
    }
    
    std::vector<LogHandler_P> GetHandlers() {
        return Logger::Inst().GetHandlers();
    }
}

