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

set(SRC_DIR src)
set(INCL_DIR include)

set(SRC_FILES
	"${PROJECT_SOURCE_DIR}/${SRC_DIR}/data_reader.c"
	"${PROJECT_SOURCE_DIR}/${SRC_DIR}/datetime.c"
	"${PROJECT_SOURCE_DIR}/${SRC_DIR}/directory.c"
	"${PROJECT_SOURCE_DIR}/${SRC_DIR}/errors.c"
	"${PROJECT_SOURCE_DIR}/${SRC_DIR}/extractor.c"
	"${PROJECT_SOURCE_DIR}/${SRC_DIR}/file_flags.c"
	"${PROJECT_SOURCE_DIR}/${SRC_DIR}/hh.c"
	"${PROJECT_SOURCE_DIR}/${SRC_DIR}/index_file.c"
	"${PROJECT_SOURCE_DIR}/${SRC_DIR}/log.c"
	"${PROJECT_SOURCE_DIR}/${SRC_DIR}/main.c"
	"${PROJECT_SOURCE_DIR}/${SRC_DIR}/options.c"
	"${PROJECT_SOURCE_DIR}/${SRC_DIR}/output.c"
	"${PROJECT_SOURCE_DIR}/${SRC_DIR}/path_table.c"
	"${PROJECT_SOURCE_DIR}/${SRC_DIR}/utils.c"
	"${PROJECT_SOURCE_DIR}/${SRC_DIR}/volume_descriptor.c"
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE ${INCL_DIR})

add_custom_target(format
    COMMAND clang-format -style=GNU -i ${SRC_FILES} ${PROJECT_SOURCE_DIR}/${INCL_DIR}/*.h
    COMMENT "Formatting code."
)
