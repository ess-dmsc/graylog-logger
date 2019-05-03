add_executable(console_logger ConsoleLogger.cpp getopt.h)

target_link_libraries(console_logger GraylogLogger::graylog_logger)

if(MSVC)
    target_include_directories(console_logger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}) #For getopt.h
endif()
