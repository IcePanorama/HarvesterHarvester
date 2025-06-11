/**
 *  iso_9660/dir_rec.h - an interface for processing and extracting ISO 9660
 *  directory records. Intended for internal usage. All public functions are
 *  prefixed by `_i9660dr`.
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
#ifndef _ISO_9660_DIRECTORY_RECORD_H_
#define _ISO_9660_DIRECTORY_RECORD_H_

#include "iso_9660/file_flags.h"

#include <stdint.h>
#include <stdio.h>

typedef struct _ISO9660DirRec_s _ISO9660DirRec_t;
extern const size_t _I9660DR_SIZE_BYTES;

/**
 *  Allocates an _ISO9660DirRec_t. Said directory record data must later be
 *  freed via `_i9660dr_free`. Return value may be NULL upon calloc failure.
 *
 *  Return:  pointer to allocated directory record
 *  See:  `_i9660dr_free`
 */
_ISO9660DirRec_t *_i9660dr_alloc (void);

/**
 *  Frees memory allocated by `dr`. Returns immediately if `dr` is NULL.
 *
 *  Param:  dr  a pointer to an _ISO9660DirRec_t
 */
void _i9660dr_free (_ISO9660DirRec_t *dr);

/**
 *  Prints all the data fields of `dr` to stdout. Returns immediately if `dr`
 *  is NULL.
 *
 *  Param:  dr  some _ISO9660DirRec_t
 */
void _i9660dr_print (_ISO9660DirRec_t *dr);

/**
 *  Initializes all the data fields of `dr` using the data provided via
 *  `input_fptr`. Returns non-zero if `dr` is NULL.
 *
 *  Param:  dr          some _ISO9660DirRec_t
 *  Param:  input_fptr  file pointer to some directory record data
 *  Return:  Zero on success, non-zero on failure.
 */
int _i9660dr_init (_ISO9660DirRec_t *dr, FILE input_fptr[static 1]);

/**
 *  Dynamically initializes a list of directory records all at once, as they're
 *  stored consecutively. Calls `_i9660dr_init` internally. `list_cap` and
 *  `list_len` pointers are needed as this function will automatically resize
 *  `dr_list` when necessary. Caller is responsible for allocating and freeing
 *  `dr_list`.
 *
 *  Param:  dr_list     pointer to a list of type _ISO9660DirRec_t
 *  Param:  list_cap    pointer to the physical size of `dr_list`
 *  Param:  list_len    pointer to the logical size of `dr_list`
 *  Param:  input_fptr  file pointer to some directory record data
 *  Returns: Zero on success, non-zero on failure.
 *  See:  `_i9660dr_init`
 */
int _i9660dr_dynamic_init (_ISO9660DirRec_t **dr_list,
                           size_t list_cap[static 1],
                           size_t list_len[static 1], const uint16_t lb_size,
                           FILE input_fptr[static 1]);

/**
 *  Extracts `dr` using the data provided via `input_fptr` to the directory
 *  pointed to by `path`. Returns non-zero if `dr` is NULL. `lb_size` is needed
 *  to navigate padding present in ISO 9660 file systems.
 *
 *  Param:  dr          some _ISO9660DirRec_t
 *  Param:  input_fptr  file pointer to some directory record data
 *  Param:  path        some directory where `dr` should be extracted.
 *  Param:  lb_size     logical block size of the associated pri vol desc data
 *  Return:  Zero on success, non-zero on failure.
 *  See:  `_ISO9660PriVolDesc_t`
 */
int _i9660dr_extract (_ISO9660DirRec_t *dr, const uint16_t lb_size,
                      FILE input_fptr[static 1], const char path[static 1],
                      const size_t path_len);

/**
 *  Returns the file flags of `dr`. Returns `(_ISO9660FileFlags_t)-1` if `dr`
 *  is NULL.
 *
 *  See: `_ISO9660FileFlags_t`
 */
_ISO9660FileFlags_t _i9660dr_get_flags (_ISO9660DirRec_t *dr);

#endif /* _ISO_9660_DIRECTORY_RECORD_H_ */
