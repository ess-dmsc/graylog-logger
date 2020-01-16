find_path(GoogleBenchmark_ROOT_DIR
  NAMES include/benchmark/benchmark.h
)

find_path(GoogleBenchmark_INCLUDE_DIR
  NAMES benchmark/benchmark.h
  HINTS ${GoogleBenchmark_ROOT_DIR}/include
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GoogleBenchmark FOUND_VAR GoogleBenchmark_FOUND REQUIRED_VARS
  GoogleBenchmark_ROOT_DIR
  GoogleBenchmark_INCLUDE_DIR
)

mark_as_advanced(
  GoogleBenchmark_ROOT_DIR
  GoogleBenchmark_INCLUDE_DIR
)

find_library(GoogleBenchmark_LIB
        NAMES benchmark
        HINTS ${GoogleBenchmark_ROOT_DIR}/lib
        )