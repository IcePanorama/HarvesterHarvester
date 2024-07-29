/**
 *  directory - the `directory` and `directory_record` datatypes and their
 *  associated functions.
 *
 *  All functions, except those with a return value of void, return zero on
 *  success and non-zero on failure.
 */
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
#ifndef _HH_DIRECTORY_H_
#define _HH_DIRECTORY_H_

#include "datetime.h"
#include "file_flags.h"

#include <stdint.h>
#include <stdlib.h>

/**
 *  A directory record. It's perhaps easier to think of this as a `file`. It is
 *  worth noting, however, that we don't differentiate betewen files and
 *  subdirectories.
 *
 *  @see https://wiki.osdev.org/ISO_9660#Directories
 */
typedef struct directory_record
{
  uint8_t record_length;
  uint8_t extended_attribute_record_length;
  uint32_t location_of_extent;
  uint32_t data_length;
  dir_datetime recording_datetime;
  file_flags file_flags;
  /** Only used by files recorded in interleaved mode */
  uint8_t file_unit_size;
  /** Only used by files recorded in interleaved mode */
  uint8_t interleave_gap_size;
  uint16_t volume_sequence_number;
  uint8_t file_identifier_length;
  char *file_identifier;
} directory_record;

/**
 *  A collection of directory records which together make a single directory.
 *
 *  @see directory_record
 */
typedef struct directory
{
  directory_record *records;
  size_t size;           //!< the max capacity of `records`.
  size_t current_record; //!< last record in `records`.
} directory;

/**
 *  Initializes a directory. By default, a directory has
 *  `DIR_STARTING_NUM_RECORDS` records.
 */
int8_t create_directory (directory *d);

/**
 *  Destroys all the directory records within a given directory before then
 *  destroying the directory itself.
 */
void destroy_directory (directory *d);

/**
 *  Adds a directory record to the given directory, adjusting the size
 *  of said directory's array of records as needed.
 */
int8_t add_record_to_directory (directory *d, directory_record *r);

/**
 *  Prints a given directory to stdout in a human-readable form.
 */
void print_directory (directory *d);

/**
 *  Prints a given directory record to the stdout in a human-readable form.
 */
void print_directory_record (directory_record *r);

/**
 *  Creates directory records for every sub-directory and file in a given
 *  directory, filling out each data field of said record using the data found
 *  in the file pointed to by `fptr`.
 */
int8_t process_directory (FILE *fptr, directory *d);

#endif
