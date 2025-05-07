# add_cppcheck_target(cpp_target [suppress_file1 suppress_file2 ...])
#
# Create a target named `${cpp_target}_cppcheck` to run cppcheck on the source
# files that are part of cpp_target, suppressing messages from files that
# follow the target name; add it as a dependency of the `cppcheck` target.
#
# Only include directories inside the current `PROJECT_SOURCE_DIR` are added.
# This function has to be called in the same file where the target is defined.
#
# External requirements:
#
#     - cppcheck
#
# Example:
#
#     # Create `newtarget` target.
#     add_executable(newtarget newtarget.h newtarget.cpp anotherfile.cpp)
#
#     # Create `newtarget_cppcheck` target and add it to `cppcheck`,
#     # suppressing messages from `anotherfile.cpp`.
#     add_cppcheck_target(newtarget anotherfile.cpp)
#

find_program(CPPCHECK cppcheck)

if(CPPCHECK)
    message(STATUS "** Found cppcheck")

    add_custom_target(cppcheck)

    function(add_cppcheck_target cpp_target)
        if(NOT TARGET ${cpp_target})
            message(WARNING "Target '${cpp_target}' does not exist. Skipping cppcheck.")
            return()
        endif()

        set(excluded_files "")
        set(suppressed_files "")
        foreach(name ${ARGN})
            get_filename_component(name ${name} REALPATH)
            list(APPEND excluded_files ${name})
            file(RELATIVE_PATH rel_path ${PROJECT_SOURCE_DIR} ${name})
            set(suppressed_files --suppress=*:${rel_path} ${suppressed_files})
        endforeach()

        get_target_property(cpp_srcs ${cpp_target} SOURCES)

        set(cpp_src_list "")
        foreach(src_file ${cpp_srcs})
            get_filename_component(src_file ${src_file} REALPATH)
            list(FIND excluded_files ${src_file} file_index)
            if(${file_index} EQUAL -1)
                list(APPEND cpp_src_list ${src_file})
            endif()
        endforeach()

        get_target_property(cpp_inc_dirs ${cpp_target} INCLUDE_DIRECTORIES)

        set(cpp_inc_list "")
        foreach(inc_dir ${cpp_inc_dirs})
            string(FIND "${inc_dir}" "${PROJECT_SOURCE_DIR}" INDEX)
            if(INDEX EQUAL 0)
                list(APPEND cpp_inc_list -I${inc_dir})
            endif()
        endforeach()

        add_custom_target(
            "${cpp_target}_cppcheck"
            COMMAND echo ""
            COMMAND echo "-- cppcheck report for ${cpp_target}: --"
            COMMAND ${CPPCHECK}
                --error-exitcode=1
                --force
                --quiet
                --inline-suppr
                --enable=missingInclude
                --suppress=missingIncludeSystem
                --relative-paths=${PROJECT_SOURCE_DIR}
                ${suppressed_files}
                ${cpp_inc_list}
                ${cpp_src_list}
            COMMAND echo "--  End of report for ${cpp_target}.  --"
            COMMAND echo ""
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )

        add_dependencies(cppcheck "${cpp_target}_cppcheck")
    endfunction()
else()
    message(WARNING "** cppcheck not found")

    function(add_cppcheck_target cpp_target)
        message(WARNING "** add_cppcheck_target: target ${cpp_target}_cppcheck not created (cppcheck not found)")
    endfunction()
endif()
