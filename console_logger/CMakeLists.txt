cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)

set(SRC ConsoleLogger.cpp)

if(MSVC)
    add_definitions(-std=c++11)
    include_directories(".") #For getopt.h
    set(SRC ${SRC} getopt.c) #For getopt.c
else()
    add_definitions(-std=c++11 -pthread)
endif()

include_directories("../include")

add_executable(console_logger ${SRC})

if(NOT TARGET graylog_logger)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../graylog_logger ${CMAKE_CURRENT_BINARY_DIR}/graylog_logger)
endif()

add_dependencies(console_logger graylog_logger)

target_link_libraries(console_logger graylog_logger)