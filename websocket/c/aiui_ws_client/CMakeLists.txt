cmake_minimum_required(VERSION 2.8)
project(aiui_demo)

set(CMAKE_C_FLAGS "-w ${CMAKE_C_FLAGS} -O0 -g3 -DCJSON_HIDE_SYMBOLS")

include_directories(src)
link_libraries(-lpthread -lm -ldl)

FILE(GLOB_RECURSE SRC_LIST
        "${CMAKE_CURRENT_LIST_DIR}/src/*.c"
        "${CMAKE_CURRENT_LIST_DIR}/src/*.h")

macro(source_group_by_dir abs_cur_dir source_files)
    if (MSVC)
        set(sgbd_cur_dir ${${abs_cur_dir}})
        foreach(sgbd_file ${${source_files}})
            string(REGEX REPLACE ${sgbd_cur_dir}/\(.*\) \\1 sgbd_fpath ${sgbd_file})
            string(REGEX REPLACE "\(.*\)/.*" \\1 sgbd_group_name ${sgbd_fpath})
            string(COMPARE EQUAL ${sgbd_fpath} ${sgbd_group_name} sgbd_nogroup)
            string(REPLACE "/" "\\" sgbd_group_name ${sgbd_group_name})
            if(sgbd_nogroup)
                set(sgbd_group_name "\\")
            endif(sgbd_nogroup)
            source_group(${sgbd_group_name} FILES ${sgbd_file})
        endforeach(sgbd_file)
    endif (MSVC)
endmacro(source_group_by_dir)

source_group_by_dir(CMAKE_CURRENT_LIST_DIR SRC_LIST)

add_definitions(-DSINGLE_MAIN)

SET(EXECUTABLE_OUTPUT_PATH ../bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

add_executable(aiui_demo ${SRC_LIST})
