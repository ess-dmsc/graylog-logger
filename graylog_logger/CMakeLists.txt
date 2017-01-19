cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)
set(graylog-logger_VERSION_MAJOR 1)
set(graylog-logger_VERSION_MINOR 0)

add_definitions(-std=c++11 -pthread)

find_package(Threads)

set(CMAKE_MACOSX_RPATH FALSE)

include_directories("../include")

file(GLOB Graylog_SRC "../src/*.cpp")

add_library(graylog_logger STATIC SHARED ${Graylog_SRC})
target_link_libraries(graylog_logger ${CMAKE_THREAD_LIBS_INIT})

file(GLOB HeaderInstall "../include/graylog_logger/*.hpp")

install(TARGETS graylog_logger DESTINATION lib)
install(FILES ${HeaderInstall} DESTINATION include/graylog_logger)