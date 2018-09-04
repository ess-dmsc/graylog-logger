find_package(Threads REQUIRED)

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

set(Graylog_INC
    ../include/graylog_logger/ConcurrentQueue.hpp
    ../include/graylog_logger/ConsoleInterface.hpp
    ../include/graylog_logger/FileInterface.hpp
    ../include/graylog_logger/GraylogConnection.hpp
    ../include/graylog_logger/GraylogInterface.hpp
    ../include/graylog_logger/Log.hpp
    ../include/graylog_logger/Logger.hpp
    ../include/graylog_logger/LoggingBase.hpp
    ../include/graylog_logger/LogUtil.hpp
)

add_library(graylog_logger SHARED ${Graylog_SRC} ${Graylog_INC})
add_library(GraylogLogger::graylog_logger ALIAS graylog_logger)
target_link_libraries(graylog_logger ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(graylog_logger
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
  PRIVATE
    "."
)

add_library(graylog_logger_static STATIC ${Graylog_SRC} ${Graylog_INC} ${Graylog_private_INC})
add_library(GraylogLogger::graylog_logger_static ALIAS graylog_logger_static)
target_link_libraries(graylog_logger_static ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(graylog_logger_static
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
  PRIVATE
    "."
)

set_target_properties(graylog_logger PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(graylog_logger_static PROPERTIES VERSION ${PROJECT_VERSION})

add_cppcheck_target(
    graylog_logger
    "../include/graylog_logger/ConcurrentQueue.hpp"
)


include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/GraylogLogger)

install(TARGETS graylog_logger graylog_logger_static
  EXPORT graylog_logger-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY ../include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT graylog_logger-targets
  FILE
    GraylogLoggerTargets.cmake
  NAMESPACE
    GraylogLogger::
  DESTINATION
    ${INSTALL_CONFIGDIR}
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/GraylogLoggerConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_MODULE_PATH}/GraylogLoggerConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/GraylogLoggerConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

install(FILES
   ${CMAKE_CURRENT_BINARY_DIR}/GraylogLoggerConfig.cmake
   ${CMAKE_CURRENT_BINARY_DIR}/GraylogLoggerConfigVersion.cmake
   DESTINATION ${INSTALL_CONFIGDIR}
)

export(
  EXPORT graylog_logger-targets
  FILE ${CMAKE_CURRENT_BINARY_DIR}/GraylogLoggerTargets.cmake
  NAMESPACE GraylogLogger::
)

export(PACKAGE GraylogLogger)
