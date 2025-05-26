/**
 *  iso_9660/fs_header.h - an interface for ISO 9660 file system headers.
 *  Intended for internal usage. All functions are prefixed by `_i9660h`. All
 *  data types are prefixed by `_ISO9660Header`.
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
#ifndef _ISO_9660_HEADER_H_
#define _ISO_9660_HEADER_H_

#include "iso_9660/vol_desc_type_code.h"

#include <stdio.h>

typedef struct _ISO9660Header_s _ISO9660Header_t;

/**
 *  Allocates an _ISO9660Header_t. Said header must later be freed via
 *  `_i9660h_free`. Return value may be NULL upon calloc failure.
 *
 *  Return:  pointer to allocated i9660 header
 *  See:  `_i9660h_free`
 */
_ISO9660Header_t *_i9660h_alloc (void);

/**
 *  Frees memory allocated by `h`. If `h` is NULL, this simply returns
 *  immediately.
 *
 *  Param:  h  a pointer to some _ISO9660Header_t
 */
void _i9660h_free (_ISO9660Header_t *h);

/**
 *  Prints all the data fields of `h` to stdout. Returns immediately if `h` is
 *  NULL.
 *
 *  Param:  h  a pointer to some _ISO9660Header_t
 */
void _i9660h_print (_ISO9660Header_t *h);

/**
 *  Initializes all the data fields of `h` using the data provided via
 *  `input_fptr`. Returns non-zero if `h` is NULL.
 *
 *  Param:  h           a pointer to some _ISO9660Header_t
 *  Param:  input_fptr  file pointer to some ISO 9660 data
 *  Return:  Zero on success, non-zero on failure.
 */
int _i9660h_init (_ISO9660Header_t *h, FILE input_fptr[static 1]);

/**
 *  Returns the volume descriptor type code of the given header, `h`. Returns
 *  `VDTC_VOL_DESC_SET_TERMINATOR` when `h` is null.
 *
 *  Param:  h  a pointer to some _ISO9660Header_t
 *  Return:  the _ISO9660VolDescTypeCode_t of `h`
 *  FIXME: Should this be in the vol desc type code file instead?
 */
_ISO9660VolDescTypeCode_t _i9660h_get_vol_desc_type_code (_ISO9660Header_t *h);

#endif /* _ISO_9660_HEADER_H_ */
