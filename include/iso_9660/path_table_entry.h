/**
 *  iso_9660/path_table_entry.h - an interface for processing and extracting
 *  ISO 9660 path tables and path table entries. Intended for internal usage.
 *  All public functions are prefixed by `_i9660pte`.
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
#ifndef _ISO_9660_PATH_TABLE_ENTRY_H_
#define _ISO_9660_PATH_TABLE_ENTRY_H_

#include <stdint.h>
#include <stdio.h>

typedef struct _I9660PathTableEntry_s _ISO9660PathTableEntry_t;
extern const size_t _I9660PTE_SIZE_BYTES;

/**
 *  Allocates an _ISO9660PathTableEntry_t. Said path table entry must later be
 *  freed via `_i9660pte_free`. Return value may be NULL upon calloc failure.
 *
 *  Return:  pointer to allocated path table entry
 *  See:  `_i9660pte_free`
 */
_ISO9660PathTableEntry_t *_i9660pte_alloc (void);

/**
 *  Frees memory allocated by `p`. Returns immediately if `p` is NULL.
 *
 *  Param:  p  a pointer to an _ISO9660PathTableEntry_t
 */
void _i9660pte_free (_ISO9660PathTableEntry_t *p);

/**
 *  Prints all the data fields of `p` to stdout. Returns immediately if `p` is
 *  NULL.
 *
 *  Param:  p  some _ISO9660PathTableEntry_t
 */
void _i9660pte_print (_ISO9660PathTableEntry_t *p);

/**
 *  Initializes all the data fields of `p` using the data provided via
 *  `input_fptr`. Returns non-zero if `p` is NULL.
 *
 *  Param:  p           some _ISO9660PathTableEntry_t
 *  Param:  input_fptr  file pointer to some path table entry data
 *  Return:  Zero on success, non-zero on failure.
 */
int _i9660pte_init (_ISO9660PathTableEntry_t *p, FILE input_fptr[static 1]);

/**
 *  Builds a list of every path table in `p`, and extracts them using
 *  `input_fptr` to the directory `path`. Returns non-zero if `p` is NULL.
 *  `lb_size` is needed to navigate padding present in ISO 9660 file systems.
 *
 *  Param:  p           some _ISO9660PathTableEntry_t
 *  Param:  lb_size     logical block size of the associated pri vol desc data
 *  Param:  input_fptr  file pointer to some path table data
 *  Param:  path        some directory where `p` should be extracted.
 *  Param:  path_len    the length of `path`
 *  Return:  Zero on success, non-zero on failure.
 */
int _i9660pte_extract (_ISO9660PathTableEntry_t *p, uint16_t lb_size,
                       FILE input_fptr[static 1], const char path[static 1],
                       const size_t path_len);

/**
 *  Returns 0 if `p` is NULL.
 *
 *  Param:  p  some _ISO9660PathTableEntry_t
 *  Return:  `dir_id_len` of `p`
 */
uint8_t _i9660pte_get_dir_id_len (const _ISO9660PathTableEntry_t *p);

/**
 *  Returns 1 when `p` is NULL, as directories are 1-indexed and therefore the
 *  directory number of the root directory is 1.
 *
 *  Param:  p  some _ISO9660PathTableEntry_t
 *  Return: `parent_dir_num` data field of `p`
 */
uint16_t _i9660pte_get_parent_dir_num (const _ISO9660PathTableEntry_t *p);

/**
 *  Param:  p  some _ISO9660PathTableEntry_t
 *  Return:  `dir_id` data field of `p`
 */
const char *_i9660pte_get_dir_id (const _ISO9660PathTableEntry_t *p);

#endif /* _ISO_9660_PATH_TABLE_ENTRY_H_ */
