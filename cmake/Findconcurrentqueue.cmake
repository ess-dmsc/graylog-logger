find_path(concurrentqueue_INCLUDE_DIR
  NAMES concurrentqueue/concurrentqueue.h
  PATHS /usr/local/include/ /opt/local/include/
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(concurrentqueue FOUND_VAR concurrentqueue_FOUND REQUIRED_VARS
  concurrentqueue_INCLUDE_DIR
)

mark_as_advanced(
  concurrentqueue_INCLUDE_DIR
)

if(concurrentqueue_FOUND)
    set(concurrentqueue_INCLUDE_DIRS ${concurrentqueue_INCLUDE_DIR})
endif()

if(concurrentqueue_FOUND AND NOT TARGET concurrentqueue::concurrentqueue)
    add_library(concurrentqueue::concurrentqueue INTERFACE IMPORTED)
    set_target_properties(concurrentqueue::concurrentqueue PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${concurrentqueue_INCLUDE_DIR}"
            )
endif()
