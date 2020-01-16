find_path(ConcurrentQueue_INCLUDE_DIR
  NAMES concurrentqueue/concurrentqueue.h
  PATHS /usr/local/include/ /opt/local/include/
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ConcurrentQueue FOUND_VAR ConcurrentQueue_FOUND REQUIRED_VARS
  ConcurrentQueue_INCLUDE_DIR
)

mark_as_advanced(
  ConcurrentQueue_INCLUDE_DIR
)

if(ConcurrentQueue_FOUND)
    set(ConcurrentQueue_INCLUDE_DIRS ${ConcurrentQueue_INCLUDE_DIR})
endif()

if(ConcurrentQueue_FOUND AND NOT TARGET ConcurrentQueue::ConcurrentQueue)
    add_library(ConcurrentQueue::ConcurrentQueue INTERFACE IMPORTED)
    set_target_properties(ConcurrentQueue::ConcurrentQueue PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${ConcurrentQueue_INCLUDE_DIR}"
            )
endif()
