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

set(Boost_FIND_REQUIRED NO)

find_package(Boost COMPONENTS system thread regex filesystem)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
    link_directories(${Boost_LIBRARY_DIRS})

    # find_package(GTest) does not find Google Mock, so find it manually.
    find_library(GMOCK_MAIN_LIB gmock_main)

    # Prevent GoogleTest from overriding our compiler/linker options
    # when building with Visual Studio
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    include_directories("../include")

    set(UnitTest_SRC
        BaseLogHandlerStandIn.hpp
        BaseLogHandlerTest.cpp
        ConsoleInterfaceTest.cpp
        FileInterfaceTest.cpp
        GraylogInterfaceTest.cpp
        LoggingBaseTest.cpp
        LogMessageTest.cpp
        LogTestServer.cpp
        LogTestServer.hpp
        QueueLengthTest.cpp
        RunTests.cpp
    )

    set(UnitTest_INC
        BaseLogHandlerStandIn.hpp
        LogTestServer.hpp
    )

    set(Graylog_SRC
        ../src/ConsoleInterface.cpp
        ../src/FileInterface.cpp
        ../src/GraylogConnection.cpp
        ../src/GraylogInterface.cpp
        ../src/jsoncpp.cpp
        ../src/Log.cpp
        ../src/Logger.cpp
        ../src/LoggingBase.cpp
        ../src/LogUtil.cpp
    )

    set(Graylog_INC
        ../include/graylog_logger/ConcurrentQueue.hpp
        ../include/graylog_logger/ConsoleInterface.hpp
        ../include/graylog_logger/FileInterface.hpp
        ../include/graylog_logger/GraylogConnection.hpp
        ../include/graylog_logger/GraylogInterface.hpp
        ../include/graylog_logger/json.h
        ../include/graylog_logger/Log.hpp
        ../include/graylog_logger/Logger.hpp
        ../include/graylog_logger/LoggingBase.hpp
        ../include/graylog_logger/LogUtil.hpp
    )

    add_executable(unit_tests ${UnitTest_SRC} ${UnitTest_INC} ${Graylog_SRC} ${Graylog_INC})

    # set(needed_features cxx_strong_enums cxx_constexpr)
    # target_compile_features(LogTests PRIVATE ${needed_features})

    target_link_libraries(unit_tests ${GMOCK_MAIN_LIB})
    target_link_libraries(unit_tests ${Boost_LIBRARIES})

    add_test(TestAll unit_tests)

else()
    message(WARNING "Unable to find boost, skipping unit tests.")
endif()
