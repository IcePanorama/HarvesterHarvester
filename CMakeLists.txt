cmake_minimum_required (VERSION 3.28)

project(HarvesterHarvester C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

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

set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(INCL_DIR "${PROJECT_SOURCE_DIR}/include")
set(BIN_DIR "${PROJECT_SOURCE_DIR}/bin")
set(HEADER_CPY_DIR "${BIN_DIR}/include/HarvesterHarvester")

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

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

add_custom_target(format
    COMMAND clang-format -style=GNU -i ${SRC_FILES} ${INCL_DIR}/*.h
    COMMENT "Auto-formatting code."
)

add_custom_target(test
    COMMAND valgrind --leak-check=yes ./${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/dat-files/HARVEST.DAT
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Testing for memory leaks with Valgrind."
)

# Copy
file(MAKE_DIRECTORY ${BIN_DIR})
file(MAKE_DIRECTORY ${HEADER_CPY_DIR})
add_custom_target(copy
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${INCL_DIR} ${HEADER_CPY_DIR}
    COMMENT "Copying header files to ${HEADER_CPY_DIR}."
)

# All
add_executable(${PROJECT_NAME} ${SRC_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCL_DIR})
add_dependencies(${PROJECT_NAME} format)

# Full
add_custom_target(full
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${PROJECT_NAME}
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target test
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target copy
    COMMENT "Running full build (clean, format, build, and test)."
)
