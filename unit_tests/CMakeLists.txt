cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

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

find_package(Boost COMPONENTS system thread regex filesystem)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
    link_directories(${Boost_LIBRARY_DIRS})

    # find_package(GTest) does not find Google Mock, so find it manually.
    find_library(GMOCK_MAIN_LIB gmock_main)

    # Prevent GoogleTest from overriding our compiler/linker options
    # when building with Visual Studio
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

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

    add_executable(unit_tests ${UnitTest_SRC} ${UnitTest_INC})

    target_link_libraries(unit_tests GraylogLogger::graylog_logger ${GMOCK_MAIN_LIB} ${Boost_LIBRARIES})

    add_test(TestAll unit_tests)

else()
    message(WARNING "Unable to find boost, skipping unit tests.")
endif()
