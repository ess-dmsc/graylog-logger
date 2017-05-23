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
    message("** Found cppcheck")

    add_custom_target(cppcheck)

    function(add_cppcheck_target cpp_target)
        # Convert extra arguments into a list and a string of arguments.
        set(ignored_files "")
        foreach(name ${ARGN})
            # Get full path to file.
            get_filename_component(name ${name} REALPATH)
            # Append to the list of files to exclude.
            list(APPEND excluded_files ${name})
            # When using --relative-paths, --suppress needs a relative path.
            file(RELATIVE_PATH rel_path ${PROJECT_SOURCE_DIR} ${name})
            # Prepare cppcheck invocation options for supression.
            set(suppressed_files --suppress=*:${rel_path} ${suppressed_files})
        endforeach()

        # Get list of target source files.
        get_target_property(cpp_srcs ${cpp_target} SOURCES)

        # Build list of source files for cppcheck invocation.
        set(cpp_src_list "")
        foreach(src_file ${cpp_srcs})
            # Get full path to file.
            get_filename_component(src_file ${src_file} REALPATH)
            # Check if file is in exclusion list.
            list(FIND excluded_files ${src_file} file_index)
            if(${file_index} EQUAL -1)
                # File is not in exclusion list, add it to list of sources.
                set(cpp_src_list ${cpp_src_list} ${src_file})
            endif()
        endforeach(src_file)

        # Get list of target include directories.
        get_target_property(cpp_inc_dirs ${cpp_target} INCLUDE_DIRECTORIES)

        # Build list of include directories for cppcheck invocation.
        set(cpp_inc_list "")
        foreach(inc_dir ${cpp_inc_dirs})
            # Check if directory is inside current project.
            string(FIND "${inc_dir}" "${PROJECT_SOURCE_DIR}" INDEX)
            if(INDEX EQUAL 0)
                # Directory is inside project, add it to includes.
                set(cpp_inc_list -I${inc_dir} ${cpp_inc_list})
            endif(INDEX EQUAL 0)
        endforeach(inc_dir)

        add_custom_target(
            "${cpp_target}_cppcheck"
            COMMAND echo ""
            COMMAND echo "-- cppcheck report for ${cpp_target}: --"
            COMMAND cppcheck
                --error-exitcode=1
                --force
                --quiet
                --inline-suppr
                --enable=style
                --enable=unusedFunction
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
    endfunction(add_cppcheck_target)
else(CPPCHECK)
    message("** cppcheck not found")

    function(add_cppcheck_target cpp_target)
        message("** add_cppcheck_target: target ${name}_cppcheck not created")
    endfunction(add_cppcheck_target)
endif(CPPCHECK)
