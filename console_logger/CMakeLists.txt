cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)

add_definitions(-std=c++11)

include_directories("../include")

add_executable(console_logger "ConsoleLogger.cpp")

add_dependencies(console_logger graylog_logger)

target_link_libraries(console_logger graylog_logger)