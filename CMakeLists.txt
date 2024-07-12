cmake_minimum_required(VERSION 3.7)
project("graylog-logger"
  VERSION 2.1.3
  DESCRIPTION "A simple logging library."  
  LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

# Conan - bring in dependencies with conan
set(CONAN_PROFILE "default" CACHE STRING "Name of conan profile to use, uses default by default")
set(CONAN "AUTO" CACHE STRING "conan options AUTO (conan must be in path), MANUAL (expects conanbuildinfo.cmake in build directory) or DISABLE")

if(${CONAN} MATCHES "AUTO")
  include(${PROJECT_SOURCE_DIR}/cmake/conan.cmake)
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
elseif(${CONAN} MATCHES "DISABLE")
    message(STATUS "***Deactivating Conan support.***")
else()
    message(FATAL_ERROR "Unrecognised option for CONAN, use AUTO, MANUAL or DISABLE")
endif()

if (NOT WIN32)
    include("CppCheck")
else()
    # Tell asio to target Windows 10
    add_definitions(-D_WIN32_WINNT=0x0A00)
endif(NOT WIN32)

find_package(asio REQUIRED)
find_package(nlohmann_json QUIET)
if (NOT nlohmann_json_FOUND)
	message(STATUS "Unable to find system installed version of jsonformoderncpp. Attempting to find Conan provided version.")
	find_package(jsonformoderncpp REQUIRED)
endif()

find_package(Threads REQUIRED)
find_package(concurrentqueue REQUIRED)

find_package(GTest)
find_package(fmt)
find_package(benchmark)

add_subdirectory(src)
add_subdirectory(console_logger)

if(fmt_FOUND)
    set(WITH_FMT 1)
endif()

configure_file(include/graylog_logger/LibConfig.hpp.in include/graylog_logger/LibConfig.hpp)

if(GTest_FOUND)
  message(STATUS "GTest found, adding unit tests.")
  add_subdirectory(unit_tests)
else()
  message(STATUS "Unable to find GTest. Skipping unit tests.")
endif()

if(benchmark_FOUND AND fmt_FOUND)
    message(STATUS "google-benchmark and fmtlib found, adding performance tests.")
    add_subdirectory(performance_test)
else()
    message(STATUS "Unable to find google-benchmark and/or fmtlib. Skipping performance tests.")
endif()

