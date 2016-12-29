cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)
project("dm-graylog-logger")
set(dm-graylog-logger_VERSION_MAJOR 1)
set(dm-graylog-logger_VERSION_MINOR 0)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/library_cmake_files)

add_subdirectory(GraylogLogger)
add_subdirectory(console_logger)
add_subdirectory(tests)
