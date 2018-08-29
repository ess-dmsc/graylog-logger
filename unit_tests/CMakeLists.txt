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

find_package(Boost COMPONENTS system filesystem)
find_package(ASIO REQUIRED)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
    link_directories(${Boost_LIBRARY_DIRS})
    
    find_package(GTest REQUIRED)
    find_library(GMOCK_MAIN_LIB gmock_main PATHS "${GTEST_ROOT}/lib")

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

    target_link_libraries(unit_tests GraylogLogger::graylog_logger ${GMOCK_MAIN_LIB} ${GTEST_LIBRARIES} ${Boost_LIBRARIES})
    target_include_directories(unit_tests PRIVATE ${ASIO_INCLUDE_DIR})

    add_test(TestAll unit_tests)

else()
    message(WARNING "Unable to find boost, skipping unit tests.")
endif()
