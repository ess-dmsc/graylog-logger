set(common_private_libs
    Threads::Threads
    asio::asio
)
if(nlohmann_json_FOUND)
    list(APPEND common_private_libs nlohmann_json::nlohmann_json)
endif()

if(jsonformoderncpp_FOUND)
    list(APPEND common_private_libs jsonformoderncpp::jsonformoderncpp)
endif()

set(common_public_libs
    concurrentqueue::concurrentqueue
)

if(fmt_FOUND)
    message(STATUS "Found fmtlib, adding support for threaded formatting.")
    list(APPEND common_public_libs fmt::fmt)
else()
    message(STATUS "Unable to find fmtlib. There will be no support for threaded formatting.")
endif()

set(Graylog_SRC
    ConsoleInterface.cpp
    FileInterface.cpp
    GraylogConnection.cpp
    GraylogInterface.cpp
    Log.cpp
    Logger.cpp
    LoggingBase.cpp
    LogUtil.cpp
)

add_library(graylog_logger SHARED ${Graylog_SRC})
add_library(GraylogLogger::graylog_logger ALIAS graylog_logger)

target_link_libraries(graylog_logger
  PRIVATE ${common_private_libs}
  PUBLIC  ${common_public_libs}
)

target_include_directories(graylog_logger
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/generated>
)

add_library(graylog_logger_static STATIC ${Graylog_SRC})
add_library(GraylogLogger::graylog_logger_static ALIAS graylog_logger_static)

target_link_libraries(graylog_logger_static
  PRIVATE ${common_private_libs}
  PUBLIC  ${common_public_libs}
)

target_include_directories(graylog_logger_static
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/generated>
)

set_target_properties(graylog_logger PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(graylog_logger_static PROPERTIES VERSION ${PROJECT_VERSION} POSITION_INDEPENDENT_CODE ON)

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/GraylogLogger)

install(TARGETS graylog_logger graylog_logger_static
    EXPORT graylog_logger-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES
  "${CMAKE_BINARY_DIR}/generated/graylog_logger/LibConfig.hpp"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/graylog_logger"
)

install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")

install(EXPORT graylog_logger-targets
    FILE GraylogLoggerTargets.cmake
    NAMESPACE GraylogLogger::
    DESTINATION ${INSTALL_CONFIGDIR}
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/GraylogLoggerConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/graylog-loggerConfig.cmake"
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/graylog-loggerConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/graylog-loggerConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/graylog-loggerConfigVersion.cmake"
    DESTINATION ${INSTALL_CONFIGDIR}
)
