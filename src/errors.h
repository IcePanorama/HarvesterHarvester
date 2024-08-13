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
/** errors.h - a wrapper for all of hh's potential errors. */
#ifndef _HH_ERRORS_H_
#define _HH_ERRORS_H_

#include <stdint.h>
#include <stdio.h>

/** An enum containing all the possible errors hh could throw. */
typedef enum HH_ERRORS
{
  HH_FOPEN_ERROR = -1,
  HH_FREAD_ERROR = -2,
  /** Path table resize error */
  HH_PT_RESIZE_ERROR = -3,
  HH_MEM_ALLOC_ERROR = -4,
  HH_CREATE_OUTPUT_DIR_ERROR = -5
} HH_ERRORS;

/** Format string for failed fopen error message. */
extern const char *FOPEN_FAILED_ERR_MSG_FMT;

/** Throws an fopen error. */
void fopen_error (char *filename);

/** Throws an fread error. */
HH_ERRORS handle_fread_error (size_t actual, size_t expected_bytes);

/** Throws an calloc error. */
HH_ERRORS handle_calloc_error (size_t size);

/** Throws an calloc error. */
HH_ERRORS handle_realloc_error (const char *arr_name, size_t old_size,
                                size_t new_size);

/** Throws an unknown command line argument error. */
void handle_unknown_command_line_argument_error (char *arg);

#endif
