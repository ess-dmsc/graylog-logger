cmake_minimum_required(VERSION 3.7)
project("graylog-logger"
  VERSION 1.2.0
  DESCRIPTION "A simple logging library."  
  LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Conan - bring in dependencies with conan
set(CONAN_PROFILE "default" CACHE STRING "Name of conan profile to use, uses default by default")
set(CONAN "AUTO" CACHE STRING "conan options AUTO (conan must be in path), MANUAL (expects conanbuildinfo.cmake in build directory) or DISABLE")

if(${CONAN} MATCHES "AUTO")
    include(${CMAKE_MODULE_PATH}/conan.cmake)
    conan_cmake_run(CONANFILE conanfile.txt
            PROFILE ${CONAN_PROFILE}
            BASIC_SETUP NO_OUTPUT_DIRS KEEP_RPATHS CMAKE_TARGETS
            BUILD_TYPE "None"
            BUILD outdated)
elseif(${CONAN} MATCHES "MANUAL")
    if(EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
        include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
        conan_basic_setup(NO_OUTPUT_DIRS KEEP_RPATHS TARGETS)
    else()
        message(FATAL_ERROR "CONAN set to MANUAL but no file named conanbuildinfo.cmake found in build directory")
    endif()
else()
    message(FATAL_ERROR "Unrecognised option for CONAN, use AUTO or MANUAL")
endif()

if (NOT WIN32)
    include("CppCheck")
else()
    # Tell asio to target Windows 10
    add_definitions(-D_WIN32_WINNT=0x0A00)
endif(NOT WIN32)

find_package(ASIO REQUIRED MODULE)

add_subdirectory(src)

option(BUILD_EVERYTHING "Build UnitTests, Console Logger and Benchmarks" ON)

IF(BUILD_EVERYTHING)
  add_subdirectory(unit_tests)
  add_subdirectory(console_logger)
  add_subdirectory(performance_test)
ENDIF()
