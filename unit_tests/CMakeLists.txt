include_directories("../src" "../tests")

enable_testing()

if (NOT CMAKE_MODULE_PATH)
    set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/../cmake_modules)
endif()

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

target_link_libraries(unit_tests
        GraylogLogger::graylog_logger_static
        CONAN_PKG::jsonformoderncpp
        CONAN_PKG::gtest)
        
add_test(TestAll unit_tests)
