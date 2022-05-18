set(common_private_libs
    Threads::Threads
    asio::asio
)
if (nlohmann_json_FOUND)
    list(APPEND common_private_libs nlohmann_json::nlohmann_json)
endif()

if (jsonformoderncpp_FOUND)
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

set(Graylog_INC
    ../include/graylog_logger/ConsoleInterface.hpp
    ../include/graylog_logger/FileInterface.hpp
    GraylogConnection.hpp
    Semaphore.hpp
    ../include/graylog_logger/GraylogInterface.hpp
    ../include/graylog_logger/Log.hpp
    ../include/graylog_logger/Logger.hpp
    ../include/graylog_logger/LoggingBase.hpp
    ../include/graylog_logger/LogUtil.hpp
    ../include/graylog_logger/ThreadedExecutor.hpp
    ../include/graylog_logger/ConnectionStatus.hpp
    ../include/graylog_logger/MinimalApply.hpp
    ${CMAKE_BINARY_DIR}/include/graylog_logger/LibConfig.hpp
)

add_library(graylog_logger SHARED ${Graylog_SRC} ${Graylog_INC})
add_library(GraylogLogger::graylog_logger ALIAS graylog_logger)
target_link_libraries(graylog_logger PRIVATE ${common_private_libs})
target_link_libraries(graylog_logger PUBLIC ${common_public_libs})
target_include_directories(graylog_logger
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
  PRIVATE
    "."
)

add_library(graylog_logger_static STATIC ${Graylog_SRC} ${Graylog_INC} ${Graylog_private_INC})
add_library(GraylogLogger::graylog_logger_static ALIAS graylog_logger_static)
target_link_libraries(graylog_logger_static PRIVATE ${common_private_libs})
target_link_libraries(graylog_logger_static PUBLIC ${common_public_libs})
target_include_directories(graylog_logger_static
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
  PRIVATE
    "."
)

set_target_properties(graylog_logger PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(graylog_logger_static PROPERTIES VERSION ${PROJECT_VERSION} POSITION_INDEPENDENT_CODE ON)

if (NOT WIN32)
    add_cppcheck_target(graylog_logger)
endif(NOT WIN32)


include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/GraylogLogger)

install(TARGETS graylog_logger graylog_logger_static
    EXPORT graylog_logger-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY ../include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_BINARY_DIR}/include/graylog_logger/LibConfig.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/graylog_logger/)

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

configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/GraylogLoggerConfig.cmake.in
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
