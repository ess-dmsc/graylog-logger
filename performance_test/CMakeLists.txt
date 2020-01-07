find_package(GoogleBenchmark REQUIRED)
find_package(fmt REQUIRED)

add_executable(performance_test PerformanceTest.cpp DummyLogHandler.cpp DummyLogHandler.h)

target_link_libraries(performance_test GraylogLogger::graylog_logger_static fmt::fmt ${GoogleBenchmark_LIB})

target_include_directories(performance_test PRIVATE ${GoogleBenchmark_INCLUDE_DIR})