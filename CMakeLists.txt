cmake_minimum_required(VERSION 3.6)
project("graylog-logger"
  VERSION 1.1.0
  DESCRIPTION "A simple logging library."  
  LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    conan_basic_setup(SKIP_RPATH NO_OUTPUT_DIRS)
endif()

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include("CppCheck")

add_subdirectory(src)

option(BUILD_EVERYTHING "Build UnitTests & Console Logger" ON)

IF(BUILD_EVERYTHING)
  add_subdirectory(unit_tests)
  add_subdirectory(console_logger)
ENDIF()
