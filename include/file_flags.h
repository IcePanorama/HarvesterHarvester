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
/** file_flags.h - file_flags data type and its associated functions. */
#ifndef _HH_FILE_FLAGS_H_
#define _HH_FILE_FLAGS_H_

#include <stdbool.h>
#include <stdint.h>

/**
 *  The file flags for a directory record.
 *
 *  @see https://wiki.osdev.org/ISO_9660#Directories
 *  @see directory_record
 */
typedef struct file_flags
{
  bool hidden;
  bool subdirectory;
  bool associated_file; //!< No clue what this means
  bool extended_attribute_record_contains_owner_and_group_permissions;
  /**
   *  If false, this directory record is not the file's final resting place;
   *  the file spans over multiple extents.
   */
  bool final_directory_record;
} file_flags;

/** Creates an instance of `file_flags`, initializing all values to false. */
file_flags create_file_flags (void);

/**
 *  Outputs a given instance of `file_flags` to stdout in a human-readable
 *  form.
 */
void print_file_flags (file_flags *ff);

#endif
