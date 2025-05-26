/**
 *  iso_9660/binary_reader.h - an interface for reading binary data from files.
 *  Intended for internal usage. All public functions are prefixed by
 *  `_i9660br`. All functions return zero on success, non-zero on failure.
 *
 *  Copyright (C) 2024-2025  IcePanorama
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef _ISO_9660_BINARY_DATA_READER_H_
#define _ISO_9660_BINARY_DATA_READER_H_

#include <stdint.h>
#include <stdio.h>

/** Reads a `uint8_t` into `output` from `fptr`. */
int _i9660br_read_u8 (FILE *fptr, uint8_t *output);

/** Reads a little-endian `uint16_t` into `output` from `fptr`. */
int _i9660br_read_le_u16 (FILE *fptr, uint16_t *output);

/** Reads a big-endian `uint16_t` into `output` from `fptr`. */
int _i9660br_read_be_u16 (FILE *fptr, uint16_t *output);

/**
 *  Reads a little-endian (LE) followed by big-endian (BE) encoded `uint16_t`
 *  into `output` from `fptr`. Double checks that the LE and BE values match
 *  because, why not?
 *
 *  See: https://wiki.osdev.org/ISO_9660#Numerical_formats
 */
int _i9660br_read_le_be_u16 (FILE *fptr, uint16_t *output);

/** Reads a little-endian `uint32_t` into `output` from `fptr`. */
int _i9660br_read_le_u32 (FILE *fptr, uint32_t *output);

/** Reads a big-endian `uint32_t` into `output` from `fptr`. */
int _i9660br_read_be_u32 (FILE *fptr, uint32_t *output);

/**
 *  Reads a little-endian (LE) followed by big-endian (BE) encoded `uint32_t`
 *  into `output` from `fptr`. Double checks that the LE and BE values match
 *  because, why not?
 *
 *  See: https://wiki.osdev.org/ISO_9660#Numerical_formats
 */
int _i9660br_read_le_be_u32 (FILE *fptr, uint32_t *output);

/**
 *  Reads a string of size `length` into `output` from `fptr`. The caller is
 *  responsible for the allocation of `output`, and for ensuring that it is at
 *  least `length` in size.
 */
int _i9660br_read_str (FILE *fptr, char *output, size_t length);

/**
 *  Reads an array of `length`-many `uint8_t`s into `output` from `fptr`. The
 *  caller is responsible for the allocation of `output`, and for ensuring that
 *  it is at least `length` in size.
 */
int _i9660br_read_u8_array (FILE *fptr, uint8_t *output, size_t length);

#endif /* _ISO_9660_BINARY_DATA_READER_H_ */
