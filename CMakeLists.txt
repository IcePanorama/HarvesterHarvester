cmake_minimum_required(VERSION 3.30)

project(HarvesterHarvester C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

if(NOT CMAKE_BUILD_TYPE MATCHES "Debug|Release")
    message(WARNING "CMAKE_BUILD_TYPE not set, defaulting to Debug.")
    set(CMAKE_BUILD_TYPE "Debug")
else ()
    message(STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}.")
endif()

add_compile_options(
    -Wpedantic
    -Wextra
    -Werror
    -Wall
    -Wstrict-aliasing=3
    -Wwrite-strings
    -Wvla
    -Wcast-align=strict
    -Wstrict-prototypes
    -Wstringop-overflow=4
    -Wshadow
    -fanalyzer
)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options (-O3)
else () # not (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options (-g -O0)
endif ()

set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(SRC_FILES
    "${SRC_DIR}/main.c"
    "${SRC_DIR}/harvester_harvester/hh.c"
    "${SRC_DIR}/harvester_harvester/idx_file.c"
    "${SRC_DIR}/harvester_harvester/known_files.c"
    "${SRC_DIR}/iso_9660.c"
    "${SRC_DIR}/iso_9660/binary_reader.c"
    "${SRC_DIR}/iso_9660/dir_rec.c"
    "${SRC_DIR}/iso_9660/header.c"
    "${SRC_DIR}/iso_9660/path_table_entry.c"
    "${SRC_DIR}/iso_9660/pri_vol_date_time.c"
    "${SRC_DIR}/iso_9660/pri_vol_desc.c"
    "${SRC_DIR}/iso_9660/utils.c"
)

set(INCL_DIR "${PROJECT_SOURCE_DIR}/include")
set(INCL_FILES
    "${INCL_DIR}/harvester_harvester/hh.h"
    "${INCL_DIR}/harvester_harvester/options.h"
    "${INCL_DIR}/harvester_harvester/idx_file.h"
    "${INCL_DIR}/harvester_harvester/known_files.h"
    "${INCL_DIR}/iso_9660.h"
    "${INCL_DIR}/iso_9660/binary_reader.h"
    "${INCL_DIR}/iso_9660/dir_rec.h"
    "${INCL_DIR}/iso_9660/header.h"
    "${INCL_DIR}/iso_9660/path_table_entry.h"
    "${INCL_DIR}/iso_9660/pri_vol_date_time.h"
    "${INCL_DIR}/iso_9660/pri_vol_desc.h"
    "${INCL_DIR}/iso_9660/utils.h"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
add_executable(${PROJECT_NAME} ${SRC_FILES})

set(ARCH_SUFFIX "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
set(EXE_NAME "${PROJECT_NAME}-${ARCH_SUFFIX}")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${EXE_NAME}")

target_include_directories(${PROJECT_NAME} PRIVATE ${INCL_DIR})

find_program(CLANG_FORMAT NAMES clang-format)
if (CLANG_FORMAT)
    add_custom_target(format
        COMMAND ${CLANG_FORMAT} -style=GNU -i ${SRC_FILES} ${INCL_FILES}
        COMMENT "Running clang-format on source files."
    )

    add_dependencies(${PROJECT_NAME} format)
else () # not CLANG_FORMAT
    message(WARNING "clang-format not found. Code formatting skipped.")
endif ()

find_program(VALGRIND NAMES valgrind)
if (VALGRIND AND (NOT WIN32))
    add_custom_target(leak_check
        COMMAND ${VALGRIND} --leak-check=yes ./${EXE_NAME}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running valgrind leak check on executable."
    )

    add_custom_target(full
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${PROJECT_NAME}
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target leak_check
        COMMENT "Running full build (clean, format, build, & test)."
    )
else () # WIN32
    if (WIN32)
        # FIXME: need a good, cross-platform way to do memory leak checks.
        message(WARNING "Windows build detected. Memory leak check skipped.")
    else ()
        message(WARNING "valgrind not found. Memory leak check skipped.")
    endif ()

    add_custom_target(full
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${PROJECT_NAME}
        COMMENT "Running full build (clean, format, build, & test)."
    )
endif ()
