find_package(benchmark REQUIRED)
find_package(fmt REQUIRED)

add_executable(performance_test EXCLUDE_FROM_ALL
    PerformanceTest.cpp
    DummyLogHandler.h
    DummyLogHandler.cpp
)

target_link_libraries(performance_test
    PRIVATE
        GraylogLogger::graylog_logger_static
        fmt::fmt
        benchmark::benchmark
        benchmark::benchmark_main
)
