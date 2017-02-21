cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)

add_definitions(-std=c++11)

if(MSVC)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

include_directories("../src" "../tests")

enable_testing()

if (NOT CMAKE_MODULE_PATH)
    set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/../cmake_modules)
endif()

if(MSVC)
    set(Boost_USE_STATIC_LIBS ON)
endif()

find_package(Boost COMPONENTS system thread regex filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

if (CMAKE_VERSION VERSION_LESS 3.2)
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
else()
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
endif()

include(DownloadProject)
download_project(PROJ                googletest
                 GIT_REPOSITORY      https://github.com/google/googletest.git
                 GIT_TAG             master
                 ${UPDATE_DISCONNECTED_IF_AVAILABLE}
)

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})

# When using CMake 2.8.11 or later, header path dependencies
# are automatically added to the gtest and gmock targets.
# For earlier CMake versions, we have to explicitly add the
# required directories to the header search path ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include"
                        "${gmock_SOURCE_DIR}/include")
endif()

include_directories("../include")

file(GLOB UnitTest_SRC "*.cpp")
file(GLOB Graylog_SRC "../src/*.cpp")

add_executable(unit_tests ${UnitTest_SRC} ${Graylog_SRC})

# set(needed_features cxx_strong_enums cxx_constexpr)
# target_compile_features(LogTests PRIVATE ${needed_features})

target_link_libraries(unit_tests gtest gmock_main)
target_link_libraries(unit_tests ${Boost_LIBRARIES})

add_test(TestAll unit_tests)