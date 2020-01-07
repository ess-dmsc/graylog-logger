#pragma once

#include <graylog_logger/LoggingBase.hpp>
#include <graylog_logger/ThreadedExecutor.hpp>

class DummyLogHandler : public Log::BaseLogHandler {
public:
  DummyLogHandler() = default;
  void addMessage(const Log::LogMessage &Message) override;
private:
  Log::ThreadedExecutor Executor;
};