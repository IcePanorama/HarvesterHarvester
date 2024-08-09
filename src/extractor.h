//  Copyright (C) 2024  IcePanorama
//  This file is a part of HarvesterHarvester.
//  HarvesterHarvester is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by the
//  Free Software Foundation, either version 3 of the License, or (at your
//  option) any later version.
//  This program is distributed in the hope that it will be useful, but WITHOUT
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//  more details.
//
//  You should have received a copy of the GNU General Public License along
//  with this program.  If not, see <https://www.gnu.org/licenses/>.
/**
 *  extractor.h - container for all file extraction functions.
 *
 *  All functions return zero on success, non-zero on failure.
 */
#ifndef _HH_EXTRACTOR_H_
#define _HH_EXTRACTOR_H_

#include "directory.h"
#include "index_file.h"
#include "path_table.h"

#include <stdint.h>
#include <stdio.h>

/**
 *  Extracts a file using its respective `directory_record` and data pointed to
 *  by `fptr`, outputting it to the location denoted by `path`.
 */
int8_t extract_file_using_dir_record (FILE *fptr, struct directory_record *dr,
                                      const char *path);
/** TODO: add documentation. */
int8_t extract_file_using_idx_entry (FILE *fptr, index_entry *idx,
                                     const char *path);

/** Creates a directory and then extracts every file in said directory. */
int8_t extract_directory (FILE *fptr, const uint16_t block_size,
                          const char *path);

/** TODO: add documentation. */
int8_t extract_index_file (index_file *idx, const char *idx_path,
                           const char *dat_path);

/**
 *  Creates directories for every directory in a given path table and
 *  then extracts all of its files. A wrapper for `extract_directory` and,
 *  therefore also, `extract_file_using_dir_record` which handles the
 * extraction of every file and subdirectory using a given path table.
 *
 *  @param block_size   logical block size (in big endian form).
 *  @see extract_directory()
 *  @see extract_file_using_dir_record()
 */
int8_t create_directories_and_extract_data_from_path_file (
    FILE *fptr, uint16_t block_size, struct path_table *pt);

#endif
