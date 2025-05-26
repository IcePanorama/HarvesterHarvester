/**
 *  iso_9660.h - an interface for processing and extracting ISO 9660/ECMA-119
 *  file systems. All public functions are prefixed by `i9660` and all public
 *  data types are prefixed by `ISO9660`.
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
#ifndef _ISO_9660_FILE_SYSTEM_H_
#define _ISO_9660_FILE_SYSTEM_H_

#include <stdio.h>

typedef struct ISO9660FileSystem_s ISO9660FileSystem_t;

/**
 *  Allocates an ISO9660FileSystem_t. Said file system must later be freed via
 *  `i9660_free`. Return value may be NULL upon calloc failure.
 *
 *  Return:  pointer to allocated i9660 file system.
 *  See:  `i9660_free`
 */
ISO9660FileSystem_t *i9660_alloc (void);

/**
 *  Frees memory allocated by `fs`. If `fs` is NULL, this simply returns
 *  immediately.
 *
 *  Param:  fs  a pointer to an ISO9660FileSystem_t
 */
void i9660_free (ISO9660FileSystem_t *fs);

/**
 *  Prints all the data fields of `fs` to stdout. Returns immediately if `fs`
 *  is NULL.
 *
 *  Param:  fs  some ISO9660FileSystem_t
 */
void i9660_print (ISO9660FileSystem_t *fs);

/**
 *  Initializes all the data fields of `fs` using the data provided via
 *  `input_fptr`. Returns non-zero if `fs` is NULL.
 *
 *  Param:  fs          some ISO9660FileSystem_t
 *  Param:  input_fptr  file pointer to some ISO 9660 data
 *  Return:  Zero on success, non-zero on failure.
 */
int i9660_init (ISO9660FileSystem_t *fs, FILE input_fptr[static 1]);

/**
 *  Extracts `fs` using the data provided via `input_fptr` to the directory
 *  pointed to by `path`. Returns non-zero if `fs` is NULL.
 *
 *  Param:  fs          some ISO9660FileSystem_t
 *  Param:  input_fptr  file pointer to some ISO 9660 data
 *  Param:  path        some directory where `fs` should be extracted.
 *  Return:  Zero on success, non-zero on failure.
 */
int i9660_extract (ISO9660FileSystem_t *fs, FILE input_fptr[static 1],
                   const char path[static 1]);

#endif /* _ISO_9660_FILE_SYSTEM_H_ */
