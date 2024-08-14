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
/** utils.h - a collection of utility functions. */
#ifndef _HH_UTILS_H_
#define _HH_UTILS_H_

#include "file_flags.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/** Swaps a `uint16_t` from little endian to big endian or vice-versa. */
uint16_t change_endianness_uint16 (uint16_t value);

/** Prepends a given `prefix` onto a given string. */
void prepend_string (char *str, const char *prefix);

/**
 *  Prepends a given `prefix` onto a given string, adding an
 *  `OP_PATH_SEPARATOR` between the two of them.
 */
int8_t prepend_path_string (char *str, const char *prefix);

/** Returns true if the given string ends in `.DAT`. */
bool is_string_dat_file (char *str);

/**
 *  Returns true if the given directory exists. `dir` is a string containing
 *  the path to said directory.
 */
bool directory_exists (const char *dir);

/**
 *  Returns true if the given file exists. `filename` is a string containing
 *  the path to said file.
 */
bool file_exists (const char *filename);

/** Returns true if the next character in the file matches `expected_ch`. */
bool peek_char_is (FILE *fptr, uint8_t expected_ch);

/** Returns true if the next character in the file is an EOF marker. */
bool peek_eof (FILE *fptr);

#endif
