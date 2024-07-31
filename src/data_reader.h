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
 *  data_header.h - container for various data access functions.
 *
 *  All of these functions return zero on success, non-zero on failure. The
 *  caller is responsible for freeing any memory which may or may not have been
 *  allocated during the use of these functions.
 */
#ifndef _DATA_READER_H_
#define _DATA_READER_H_

#include "datetime.h"
#include "file_flags.h"

#include <stdint.h>
#include <stdio.h>

/**
 *  Reads in a single uint8_t from a given file, storing the output in `value`.
 */
int8_t read_single_uint8 (FILE *fptr, uint8_t *value);

/**
 *  Reads a uint16_t stored in little endian format from the file pointed to by
 *  `fptr` and stores the result in `output`.
 */
int8_t read_little_endian_data_uint16_t (FILE *fptr, uint16_t *output);

/**
 *  Reads a uint32_t stored in little endian format from the file pointed to by
 *  `fptr` and stores the result in `output`.
 */
int8_t read_little_endian_data_uint32_t (FILE *fptr, uint32_t *output);

/**
 *  When data in a given file is presented first in little endian form and then
 *  repeated in big endian form, this function will read both values, double
 *  checking that the first matches the second, and storing the result into
 *  `output`. This is almost certainly unnecessary, but it makes me feel
 *  better :).
 *
 *  @see read_both_endian_data_unint32()
 */
int8_t read_both_endian_data_uint16 (FILE *fptr, uint16_t *output);

/**
 *  When data in a given file is presented first in little endian form and then
 *  repeated in big endian form, this function will read both values, double
 *  checking that the first matches the second, and and storing the result into
 *  `output`. This is almost certainly unnecessary, but it makes me feel
 *  better :).
 *
 *  @see read_both_endian_data_unint16()
 */
int8_t read_both_endian_data_uint32 (FILE *fptr, uint32_t *output);

/**
 *  Reads a string of a given length from a given file, storing the result in
 *  `output`.
 */
int8_t read_string (FILE *fptr, char *output, uint8_t length);

/**
 *  Reads an array of uint8_t's, of a given length, from a given file,
 *  storing the result in `output`.
 */
int8_t read_array_uint8 (FILE *fptr, uint8_t *arr, uint8_t length);

/**
 *  Reads in `dec_datetime` data from a given file, storing the output in `dt`.
 *
 *  @see dec_datetime
 */
int8_t read_dec_datetime (FILE *fptr, dec_datetime *dt);

/**
 *  Reads in `dir_datetime` data from a given file, storing the output in `dt`.
 *
 *  @see dir_datetime
 */
int8_t read_dir_datetime (FILE *fptr, dir_datetime *dt);

/**
 *  Reads in `file_flag` data from a given file, storing the output in `ff`.
 *
 *  @see  file_flags
 */
int8_t read_file_flags (FILE *fptr, file_flags *ff);

#endif
