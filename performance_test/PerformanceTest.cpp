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
    Logger.log(Log::Severity(Distribution(Generator)), "Some test string.");
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_RandomSeverityLevel);

//#include "spdlog/spdlog.h"
//#include "spdlog/sinks/null_sink.h"
//#include "spdlog/async.h"
//
// static void BM_SpdLogWithFmtAndSeverityLvl(benchmark::State &state) {
//  Log::LoggingBase Logger;
//  auto null_sink = std::make_shared<spdlog::sinks::null_sink_mt>();
//  auto tp = std::make_shared<spdlog::details::thread_pool>(4096, 1);
//  auto logger = std::make_shared<spdlog::async_logger>("async_logger",
//  std::move(null_sink), std::move(tp),
//  spdlog::async_overflow_policy::overrun_oldest);
//  std::random_device Device;
//  std::mt19937 Generator(Device());
//  std::uniform_int_distribution<> Distribution(0, 6);
//  logger->set_level(spdlog::level::level_enum(3));
//  for (auto _ : state) {
//    logger->log(spdlog::level::level_enum(Distribution(Generator)), "Hello -
//    {} - {} - {}", 42, 3.14, "test");
//  }
//  state.SetItemsProcessed(state.iterations());
//}
// BENCHMARK(BM_SpdLogWithFmtAndSeverityLvl);

static void BM_GraylogWithFmtAndSeverityLvl(benchmark::State &state) {
  Log::LoggingBase Logger;
  Logger.setMinSeverity(Log::Severity(3));
  auto Handler = std::make_shared<DummyLogHandler>();
  Logger.addLogHandler(std::dynamic_pointer_cast<Log::BaseLogHandler>(Handler));
  std::random_device Device;
  std::mt19937 Generator(Device());
  std::uniform_int_distribution<> Distribution(0, 6);
  for (auto _ : state) {
    Logger.fmt_log(Log::Severity(Distribution(Generator)),
                   "Hello - {} - {} - {}", 42, 3.14, "test");
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_GraylogWithFmtAndSeverityLvl);

BENCHMARK_MAIN();
