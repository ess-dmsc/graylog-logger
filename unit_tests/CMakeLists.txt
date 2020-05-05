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
    Semaphore.hpp)

add_executable(unit_tests EXCLUDE_FROM_ALL ${UnitTest_SRC} ${UnitTest_INC})
set(unit_test_libs PUBLIC 
    GraylogLogger::graylog_logger_static
    GTest::GTest
    ${GMOCK_BOTH_LIBRARIES}
    asio::asio
)

if (nlohmann_json_FOUND)
    list(APPEND unit_test_libs nlohmann_json::nlohmann_json)
endif()

if (jsonformoderncpp_FOUND)
    list(APPEND unit_test_libs jsonformoderncpp::jsonformoderncpp)
endif()

target_link_libraries(unit_tests
    ${unit_test_libs}
    )
        
add_test(TestAll unit_tests)
