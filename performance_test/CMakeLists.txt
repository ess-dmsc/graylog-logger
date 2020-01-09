add_executable(performance_test EXCLUDE_FROM_ALL PerformanceTest.cpp DummyLogHandler.h DummyLogHandler.cpp)

target_link_libraries(performance_test GraylogLogger::graylog_logger_static fmt::fmt ${GoogleBenchmark_LIB})

target_include_directories(performance_test PRIVATE ${GoogleBenchmark_INCLUDE_DIR})