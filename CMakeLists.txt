cmake_minimum_required (VERSION 3.28)

project(HarvesterHarvester C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

set(WIN32_CC i686-w64-mingw32-gcc)
set(WIN64_CC x86_64-w64-mingw32-gcc)

add_compile_options(
    -std=c99
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

set(AUTO_FMT clang-format)
set(CODE_STYLE GNU)

set(INCL_DIR "${PROJECT_SOURCE_DIR}/include")
set(BIN_DIR "${PROJECT_SOURCE_DIR}/bin")
set(HEADER_CPY_DIR "${BIN_DIR}/include/${PROJECT_NAME}")
set(DOXY_CONFIG "${PROJECT_SOURCE_DIR}/docs/doxygen-config")

set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(SRC_FILES
	"${SRC_DIR}/data_reader.c"
	"${SRC_DIR}/datetime.c"
	"${SRC_DIR}/directory.c"
	"${SRC_DIR}/errors.c"
	"${SRC_DIR}/extractor.c"
	"${SRC_DIR}/file_flags.c"
	"${SRC_DIR}/hh.c"
	"${SRC_DIR}/index_file.c"
	"${SRC_DIR}/log.c"
	"${SRC_DIR}/main.c"
	"${SRC_DIR}/options.c"
	"${SRC_DIR}/output.c"
	"${SRC_DIR}/path_table.c"
	"${SRC_DIR}/utils.c"
	"${SRC_DIR}/volume_descriptor.c"
)

set(LIB_DIR "${BIN_DIR}/lib/${PROJECT_NAME}")
set(LIB_NAME "hh")
set(LIB_SRC_FILES
	"${SRC_DIR}/data_reader.c"
	"${SRC_DIR}/datetime.c"
	"${SRC_DIR}/directory.c"
	"${SRC_DIR}/errors.c"
	"${SRC_DIR}/extractor.c"
	"${SRC_DIR}/file_flags.c"
	"${SRC_DIR}/hh.c"
	"${SRC_DIR}/index_file.c"
	"${SRC_DIR}/log.c"
	"${SRC_DIR}/options.c"
	"${SRC_DIR}/output.c"
	"${SRC_DIR}/path_table.c"
	"${SRC_DIR}/utils.c"
	"${SRC_DIR}/volume_descriptor.c"
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

# Format target
add_custom_target(format
    COMMAND ${AUTO_FMT} -style=${CODE_STYLE} -i ${SRC_FILES} "${INCL_DIR}/*.h"
    COMMENT "Auto-formatting code."
)

# Test target
add_custom_target(test
    COMMAND valgrind --leak-check=yes ./${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/dat-files/HARVEST.DAT
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Testing for memory leaks with Valgrind."
)

# Copy target
file(MAKE_DIRECTORY ${BIN_DIR})
file(MAKE_DIRECTORY ${HEADER_CPY_DIR})
add_custom_target(copy
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${INCL_DIR} ${HEADER_CPY_DIR}
    COMMENT "Copying header files to ${HEADER_CPY_DIR}."
)

# Docs target
add_custom_target(docs
    COMMAND doxygen ${DOXY_CONFIG}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Creating documentation."
)

# Default/All target
add_executable(${PROJECT_NAME} ${SRC_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCL_DIR})
add_dependencies(${PROJECT_NAME} format)

# Library target
#FIXME: library should not be deleted by clean
file(MAKE_DIRECTORY ${LIB_DIR})
add_library(${LIB_NAME} STATIC EXCLUDE_FROM_ALL ${LIB_SRC_FILES})
set_target_properties(${LIB_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR}
)
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
add_custom_target(library DEPENDS ${LIB_NAME})

# `full` target
add_custom_target(full
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${PROJECT_NAME}
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target test
    COMMENT "Running full build (clean, format, build, and test)."
)

# Win32 target
add_custom_target(win32
    COMMAND ${WIN32_CC} ${CMAKE_C_FLAGS} -o "${PROJECT_NAME}_i686.exe" "${SRC_DIR}/*.c" -I${INCL_DIR}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Building 32-bit executable."
)

# Win64 target
add_custom_target(win64
    COMMAND ${WIN64_CC} ${CMAKE_C_FLAGS} -o "${PROJECT_NAME}_x86_64.exe" "${SRC_DIR}/*.c" -I${INCL_DIR}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Building 64-bit executable."
)

# Release target
add_custom_target(release
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${PROJECT_NAME}
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target win32
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target win64
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target library
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target docs
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target copy
    COMMENT "Running full build (clean, format, build, and test)."
)

# Clean target
set(extra_files_to_clean
    "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}_i686.exe"
    "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}_x86_64.exe"
)
set_property(
	TARGET ${PROJECT_NAME}
	APPEND
	PROPERTY ADDITIONAL_CLEAN_FILES ${extra_files_to_clean}
)
