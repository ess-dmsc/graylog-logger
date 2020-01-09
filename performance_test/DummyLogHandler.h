#pragma once

#include <graylog_logger/LogUtil.hpp>
#include <graylog_logger/ThreadedExecutor.hpp>

class DummyLogHandler : public Log::BaseLogHandler {
public:
  DummyLogHandler() = default;
  void addMessage(const Log::LogMessage &Message) override;
  bool emptyQueue() override { return true; }
  size_t queueSize() override { return 0; }
  bool flush(std::chrono::system_clock::duration) override { return true; }

private:
  Log::ThreadedExecutor Executor;
};