cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)

add_definitions(-std=c++11 -pthread)

include_directories("../src" "../tests")

enable_testing()

find_package(Boost COMPONENTS system thread regex filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

find_package(GTest REQUIRED)
find_package(GMock REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS} "../include")

file(GLOB UnitTest_SRC "*.cpp")
file(GLOB Graylog_SRC "../src/*.cpp")

add_executable(unit_tests ${UnitTest_SRC} ${Graylog_SRC})

# set(needed_features cxx_strong_enums cxx_constexpr)
# target_compile_features(LogTests PRIVATE ${needed_features})

target_link_libraries(unit_tests ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES})
target_link_libraries(unit_tests ${Boost_LIBRARIES})

add_test(TestAll unit_tests)