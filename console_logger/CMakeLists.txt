cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)

add_definitions(-std=c++11)

if(MSVC)
    include_directories(".") #For getopt.h
endif()

include_directories("../include")

add_executable(console_logger ConsoleLogger.cpp)

if(NOT TARGET graylog_logger)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../graylog_logger ${CMAKE_CURRENT_BINARY_DIR}/graylog_logger)
endif()

add_dependencies(console_logger graylog_logger_static)

target_link_libraries(console_logger graylog_logger_static)