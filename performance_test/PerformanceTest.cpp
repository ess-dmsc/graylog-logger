#include <benchmark/benchmark.h>
#include <graylog_logger/LoggingBase.hpp>
#include <graylog_logger/LogUtil.hpp>
#include <graylog_logger/ThreadedExecutor.hpp>
#include "DummyLogHandler.h"

static void BM_LogMessageGenerationOnly(benchmark::State& state) {
  Log::LoggingBase Logger;
  for (auto _ : state) {
    Logger.log(Log::Severity::Error, "Some message.");
  }
}
BENCHMARK(BM_LogMessageGenerationOnly);

static void BM_LogMessageGenerationWithDummySink(benchmark::State& state) {
  Log::LoggingBase Logger;
  auto Handler = std::make_shared<DummyLogHandler>();
  Logger.addLogHandler(std::dynamic_pointer_cast<Log::BaseLogHandler>(Handler));
  for (auto _ : state) {
    Logger.log(Log::Severity::Error, "Some message.");
  }
}
BENCHMARK(BM_LogMessageGenerationWithDummySink);

BENCHMARK_MAIN();
