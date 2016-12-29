cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)

add_definitions(-std=c++11)

include_directories("../include")

add_executable(ConsoleLogger "ConsoleLogger.cpp")

add_dependencies(ConsoleLogger graylog_logger)

target_link_libraries(ConsoleLogger graylog_logger)