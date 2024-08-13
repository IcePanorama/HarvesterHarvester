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
 *  path_table.h - the `path_table` and `path_table_entry` data types and their
 *  related functions.
 *
 *  All functions, except those which are void, return zero on success and
 *  non-zero on failure.
 */
#ifndef _HH_PATH_TABLE_H_
#define _HH_PATH_TABLE_H_

#include <stdint.h>
#include <stdio.h>

/**
 *  An entry of some path table.
 *
 *  @see  https://wiki.osdev.org/ISO_9660#The_Path_Table
 */
typedef struct path_table_entry
{
  uint8_t directory_identifier_length;
  uint8_t extended_attribute_record_length;
  uint32_t location_of_extent;
  uint16_t parent_directory_number; //!< indexed from 1
  char *directory_identifier;
} path_table_entry;

/** A collection of path_table_entries. */
typedef struct path_table
{
  path_table_entry *entries;
  size_t size;          //!< max size of `entries`.
  size_t current_entry; //!< pointer to most recent item in `entries`.
} path_table;

/**
 *  Initializes a given `path_table`. By default, `entries` is given a size of
 *  `PT_STARTING_NUM_ENTRIES`. Note that `PT_STARTING_NUM_ENTRIES` is defined
 *  in `path_table.c`.
 */
int8_t create_path_table (path_table *pt);

/**
 *  Destroys the given `path_table`, freeing any and all memory alloc'd for
 *  `entries`.
 */
void destroy_path_table (path_table *pt);

/**
 *  Prints the given path table out to the stdout, formatting it into a
 *  human-readible form.
 */
void print_path_table (path_table *pt);

/**
 *  Processes the given `path_table` as a Type-L `path_table`, using the data
 *  pointed to by `fptr`.
 */
int8_t process_type_l_path_table (FILE *fptr, path_table *pt);

#endif
