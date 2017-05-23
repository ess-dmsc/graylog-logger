cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)
set(graylog-logger_VERSION_MAJOR 1)
set(graylog-logger_VERSION_MINOR 0)

if(MSVC)
    add_definitions(-std=c++11)
else()
    add_definitions(-std=c++11 -pthread)
endif()

find_package(Threads)

set(CMAKE_MACOSX_RPATH FALSE)

include_directories("../include")

set(Graylog_SRC
    ../src/ConsoleInterface.cpp
    ../src/FileInterface.cpp
    ../src/GraylogConnection.cpp
    ../src/GraylogInterface.cpp
    ../src/jsoncpp.cpp
    ../src/Log.cpp
    ../src/Logger.cpp
    ../src/LoggingBase.cpp
    ../src/LogUtil.cpp
)

set(Graylog_INC
    ../include/graylog_logger/ConcurrentQueue.hpp
    ../include/graylog_logger/ConsoleInterface.hpp
    ../include/graylog_logger/FileInterface.hpp
    ../include/graylog_logger/GraylogConnection.hpp
    ../include/graylog_logger/GraylogInterface.hpp
    ../include/graylog_logger/json.h
    ../include/graylog_logger/Log.hpp
    ../include/graylog_logger/Logger.hpp
    ../include/graylog_logger/LoggingBase.hpp
    ../include/graylog_logger/LogUtil.hpp
)

add_library(graylog_logger SHARED ${Graylog_SRC} ${Graylog_INC})
add_library(graylog_logger_static STATIC ${Graylog_SRC} ${Graylog_INC})
target_link_libraries(graylog_logger ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(graylog_logger_static ${CMAKE_THREAD_LIBS_INIT})

add_cppcheck_target(
    graylog_logger
    "../src/jsoncpp.cpp"
    "../include/graylog_logger/json.h"
)

file(GLOB HeaderInstall "../include/graylog_logger/*.hpp")

install(TARGETS graylog_logger DESTINATION lib)
install(TARGETS graylog_logger_static DESTINATION lib)
install(FILES ${HeaderInstall} DESTINATION include/graylog_logger)
