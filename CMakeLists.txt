cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)
project("graylog-logger")

set(graylog-logger_VERSION_MAJOR 1)
set(graylog-logger_VERSION_MINOR 0)
set(graylog-logger_VERSION_PATCH 4)

if(EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    conan_basic_setup(SKIP_RPATH NO_OUTPUT_DIRS)
endif()

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules)

include("CppCheck")

add_subdirectory(graylog_logger)
add_subdirectory(console_logger)

option(BUILD_EVERYTHING "Build UnitTests & Console Logger" ON)

IF(BUILD_EVERYTHING)
	add_subdirectory(unit_tests)
ENDIF()
