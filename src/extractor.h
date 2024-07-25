#ifndef _EXTRACTOR_H_
#define _EXTRACTOR_H_

#include "directory.h"
#include "path_table.h"

#include <stdint.h>
#include <stdio.h>

/**
 *  Extracts a file pointed to by `fptr` using its respective
 *  `directory_record`, outputting it to the location pointed to by `path`.
 *
 *  @param  fptr    pointer to data of the file.
 *  @param  dr      the directory record corresponding to that file.
 *  @param  path    a string of the file's output path.
 *  @return zero on success, non-zero on failure.
 */
int8_t extract_file (FILE *fptr, struct directory_record *dr,
                     const char *path);

/**
 *  Creates a directory and then extracts every file in said directory.
 *
 *  @param  fptr        pointer to the data of the directory.
 *  @param  BLOCK_SIZE  logical block size in big endian form, needed for fseek
 *  @param  path        a string of the path to output to.
 *  @return zero on success, non-zero on failure.
 */
int8_t extract_directory (FILE *fptr, const uint16_t BLOCK_SIZE,
                          const char *path);

/**
 *  A wrapper for `extract_directory` and, therefore also, `extract_file` which
 *  handles the extraction of every file and subdirectory using a given path
 *  table. Creates directories for every directory in a given path table and
 *  then extracts all of its files.
 *
 *  @param  fptr        pointer to the data
 *  @param  BLOCK_SIZE  logical block size in big endian form, needed for fseek
 *  @param  path        a string of the path to output to.
 *  @return zero on success, non-zero on failure.
 *  @see extract_directory()
 *  @see extract_file()
 */
int8_t create_directories_and_extract_data_from_path_file (
    FILE *fptr, uint16_t BLOCK_SIZE, struct path_table *pt);

#endif
