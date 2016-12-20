cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)

add_definitions(-std=c++11 -pthread)

include_directories("../src" "../tests")

enable_testing()

find_package(Boost COMPONENTS system thread regex REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

find_package(GTest)
include_directories(${GTEST_INCLUDE_DIRS} "../include")

file(GLOB UnitTest_SRC "*.cpp")

add_executable(LogTests ${UnitTest_SRC} "../src/LogUtil.cpp" "../src/LoggingBase.cpp")

# set(needed_features cxx_strong_enums cxx_constexpr)
# target_compile_features(LogTests PRIVATE ${needed_features})

target_link_libraries(LogTests ${GTEST_BOTH_LIBRARIES})
target_link_libraries(LogTests ${Boost_LIBRARIES})

add_test(TestAll LogTests)