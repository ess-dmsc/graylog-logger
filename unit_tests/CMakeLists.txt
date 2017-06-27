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

    if (CMAKE_VERSION VERSION_LESS 3.2)
        set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
    else()
        set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
    endif()

    # Tests end with segmentation fault when Google Test is built with
    # BUILD_SHARED_LIBS=ON.
    if(DEFINED CONAN_PACKAGE_NAME)
        set(BUILD_SHARED_LIBS OFF)
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

    target_link_libraries(unit_tests gtest gmock_main)
    target_link_libraries(unit_tests ${Boost_LIBRARIES})

    add_test(TestAll unit_tests)

else()
    message(WARNING "Unable to find boost, skipping unit tests.")
endif()
