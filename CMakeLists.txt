cmake_minimum_required(VERSION 3.15)

project(graylog-logger
    VERSION 2.1.6
    DESCRIPTION "A simple logging library."
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CppCheck)

find_package(asio REQUIRED)
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    message(STATUS "System nlohmann_json not found. Trying Conan's jsonformoderncpp.")
    find_package(jsonformoderncpp REQUIRED)
endif()

find_package(Threads REQUIRED)
find_package(concurrentqueue REQUIRED)
find_package(GTest)
find_package(fmt)
find_package(benchmark)

# Configure generated header before src
set(GENERATED_INCLUDE_DIR "${CMAKE_BINARY_DIR}/generated")
file(MAKE_DIRECTORY "${GENERATED_INCLUDE_DIR}/graylog_logger")

if(fmt_FOUND)
    set(WITH_FMT 1)
endif()

configure_file(
    "${CMAKE_SOURCE_DIR}/include/graylog_logger/LibConfig.hpp.in"
    "${GENERATED_INCLUDE_DIR}/graylog_logger/LibConfig.hpp"
)

if(NOT EXISTS "${GENERATED_INCLUDE_DIR}/graylog_logger/LibConfig.hpp")
    message(FATAL_ERROR "Failed to generate LibConfig.hpp")
endif()

add_subdirectory(src)
add_subdirectory(console_logger)

# Unit tests
enable_testing()
if(GTest_FOUND)
    message(STATUS "GTest found, adding unit tests.")
    add_subdirectory(unit_tests)
else()
    message(STATUS "Unable to find GTest. Skipping unit tests.")
endif()

# Performance tests
if(benchmark_FOUND AND fmt_FOUND)
    message(STATUS "google-benchmark and fmtlib found, adding performance tests.")
    add_subdirectory(performance_test)
else()
    message(STATUS "Unable to find google-benchmark and/or fmtlib. Skipping performance tests.")
endif()
