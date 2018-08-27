add_executable(console_logger ConsoleLogger.cpp getopt.h)

target_link_libraries(console_logger GraylogLogger::graylog_logger)

if(MSVC)
    target_include_directories(console_logger ".") #For getopt.h
endif()