cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)
project("dm-graylog-logger")
set(dm-graylog-logger_VERSION_MAJOR 1)
set(dm-graylog-logger_VERSION_MINOR 0)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules)

add_subdirectory(graylog_logger)

option(GRAYLOG_LOGGER_STANDALONE "Build UnitTests & Console Logger" ON)

IF(GRAYLOG_LOGGER_STANDALONE)
	add_subdirectory(console_logger)
	add_subdirectory(unit_tests)
ENDIF()
