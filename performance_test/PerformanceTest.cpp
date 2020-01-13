/* Copyright (C) 2020 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Code for running performance-tests on the library.
///
//===----------------------------------------------------------------------===//

#include "DummyLogHandler.h"
#include <benchmark/benchmark.h>
#include <fmt/format.h>
#include <graylog_logger/LoggingBase.hpp>
#include <random>

static void BM_LogMessageGenerationOnly(benchmark::State &state) {
  Log::LoggingBase Logger;
  for (auto _ : state) {
    Logger.log(Log::Severity::Error, "Some message.");
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_LogMessageGenerationOnly);

static void BM_LogMessageGenerationWithDummySink(benchmark::State &state) {
  Log::LoggingBase Logger;
  auto Handler = std::make_shared<DummyLogHandler>();
  Logger.addLogHandler(std::dynamic_pointer_cast<Log::BaseLogHandler>(Handler));
  for (auto _ : state) {
    Logger.log(Log::Severity::Error, "Some message.");
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_LogMessageGenerationWithDummySink);

static void BM_LogMessageGenerationWithFmtFormatting(benchmark::State &state) {
  Log::LoggingBase Logger;
  auto Handler = std::make_shared<DummyLogHandler>();
  Logger.addLogHandler(std::dynamic_pointer_cast<Log::BaseLogHandler>(Handler));
  for (auto _ : state) {
    Logger.log(Log::Severity::Error,
               fmt::format("Some format example: {} : {} : {}.", 3.14, 2.72,
                           "some_string"));
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_LogMessageGenerationWithFmtFormatting);

static void
BM_LogMessageGenerationWithDeferredFmtFormatting(benchmark::State &state) {
  Log::LoggingBase Logger;
  auto Handler = std::make_shared<DummyLogHandler>();
  Logger.addLogHandler(std::dynamic_pointer_cast<Log::BaseLogHandler>(Handler));
  for (auto _ : state) {
    Logger.fmt_log(Log::Severity::Error, "Some format example: {} : {} : {}.",
                   3.14, 2.72, "some_string");
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_LogMessageGenerationWithDeferredFmtFormatting);

static void BM_RandomSeverityLevel(benchmark::State &state) {
  Log::LoggingBase Logger;
  Logger.setMinSeverity(Log::Severity::Alert);
  auto Handler = std::make_shared<DummyLogHandler>();
  Logger.addLogHandler(std::dynamic_pointer_cast<Log::BaseLogHandler>(Handler));
  std::random_device Device;
  std::mt19937 Generator(Device());
  std::uniform_int_distribution<> Distribution(0, 7);
  for (auto _ : state) {
    Logger.log(Log::Severity(Distribution(Generator)), "Some format example");
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_RandomSeverityLevel);

BENCHMARK_MAIN();
