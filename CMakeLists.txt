cmake_minimum_required(VERSION 3.9)
project("graylog-logger"
  VERSION 1.1.0
  DESCRIPTION "A simple logging library."  
  LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Conan - bring in dependencies with conan
SET(CONAN_PROFILE "default" CACHE STRING "Name of conan profile to use, uses default by default")
SET(CONAN "AUTO" CACHE STRING "conan options AUTO (conan must be in path), MANUAL (expects conanbuildinfo.cmake in build directory) or DISABLE")

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
        MESSAGE(FATAL_ERROR "CONAN set to MANUAL but no file named conanbuildinfo.cmake found in build directory")
    endif()
else()
    MESSAGE(FATAL_ERROR "Unrecognised option for CONAN, use AUTO or MANUAL")
endif()

if (NOT WIN32)
    include("CppCheck")
endif(NOT WIN32)

add_subdirectory(src)

option(BUILD_EVERYTHING "Build UnitTests & Console Logger" ON)

IF(BUILD_EVERYTHING)
  add_subdirectory(unit_tests)
  add_subdirectory(console_logger)
ENDIF()
