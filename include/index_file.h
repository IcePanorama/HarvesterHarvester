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
 *  index_file.h - index_file datatype and its associated functions.
 *
 *  Used for processing Harvester's `INDEX.00#` files.
 *  <p>Both the index_file datatype and the functions contained herein are
 * based on my own observations from studying the `INDEX.00#` files and their
 *  associated `.DAT` files in a hexeditor.
 *  <p>Through these observations alone, I believe we have enough data to
 *  successfully extract files, however, it's worth noting that there is a fair
 *  bit of data that we're ignoring here.
 *  <p>Between the `filename` and `start`, there's occasionally some data, but
 *  I can't for the life of me seem to figure out what it's used for. It could
 *  be random garbage generated by whatever program created these
 *  `.DAT`/`INDEX.00#` files, or they could be file flags, or perhaps even
 *  some sort of primative checksum; I'm not sure.
 *  <p>If you have any idea what this data might be used for, feel free to make
 *  whatever changes are needed!
 */
#ifndef _HH_INDEX_FILE_H_
#define _HH_INDEX_FILE_H_

#include <stdint.h>
#include <stdio.h>

/** Both of these are probably too large, but better to be safe than sorry. */
#define FULL_PATH_MAX_LEN 128
#define FILENAME_MAX_LEN 32

/** A file entry in an `index_file`. */
typedef struct index_entry
{
  uint32_t entry_start; //!< It's location in the `INDEX.00#` file.
  char full_path[FULL_PATH_MAX_LEN];
  char filename[FILENAME_MAX_LEN];
  uint32_t start; //!< It's location in the `*.DAT` file.
  uint32_t size;
} index_entry;

/** Contains all the indicies within an `INDEX.00#` file. */
typedef struct index_file
{
  index_entry *indicies;
  size_t size;          //!< max size of `indicies`.
  size_t current_index; //!< pointer to last item in `indicies`.
} index_file;

/** Initializes an `index_file`. */
int8_t create_index_file (index_file *i);

/** Destroys a given `index_file`. */
int8_t destroy_index_file (index_file *i);

/**
 *  Reads data from a given file, creating `index_entry`s for every entry in
 *  said file.
 */
int8_t process_index_file (FILE *fptr, index_file *idxf);

/**
 *  Prints all the attributes of a given `index_entry` in a human-readable
 *  form.
 */
void print_index_entry (index_entry *idxe);

/** Adds a given `index_entry` to a `index_file`. */
int8_t add_entry_to_index_file (index_file *file, index_entry *entry);

#endif
