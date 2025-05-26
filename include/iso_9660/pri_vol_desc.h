/**
 *  iso_9660/pri_vol_desc.h - an interface for ISO 9660 primary volume
 *  descriptor data. Intended for internal usage. All public functions are
 *  prefixed by `_pvd`.
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
#ifndef _ISO_9660_PRIMARY_VOLUME_DESCRIPTOR_DATA_H_
#define _ISO_9660_PRIMARY_VOLUME_DESCRIPTOR_DATA_H_

#include <stdio.h>

typedef struct _ISO9660PriVolDesc_s _ISO9660PriVolDesc_t;

/**
 *  Allocates an _ISO9660PriVolDesc_t. Said primary volume descriptor data must
 *  later be freed via `i9660_free`. Return value may be NULL upon calloc
 *  failure.
 *
 *  Return:  pointer to allocated i9660 file system.
 *  See:  `_i9660pvd_free`
 */
_ISO9660PriVolDesc_t *_i9660pvd_alloc (void);

/**
 *  Frees memory allocated by `p`. If `p` is NULL, this simply returns
 *  immediately.
 *
 *  Param:  p  a pointer to an _ISO9660PriVolDesc_t
 */
void _i9660pvd_free (_ISO9660PriVolDesc_t *p);

/**
 *  Prints all the data fields of `p` to stdout. Returns immediately if `p` is
 *  NULL.
 *
 *  Param:  p  some _ISO9660PriVolDesc_t
 */
void _i9660pvd_print (_ISO9660PriVolDesc_t *p);

/**
 *  Initializes all the data fields of `p` using the data provided via
 *  `input_fptr`. Returns non-zero if `p` is NULL.
 *
 *  Param:  p           some _ISO9660PriVolDesc_t
 *  Param:  input_fptr  file pointer to some primary volume descriptor data
 *  Return:  Zero on success, non-zero on failure.
 */
int _i9660pvd_init (_ISO9660PriVolDesc_t *p, FILE input_fptr[static 1]);

/**
 *  Extracts `p` using the data provided via `input_fptr` to the directory
 *  pointed to by `path`. Returns non-zero if `p` is NULL.
 *
 *  Param:  p           some _ISO9660PriVolDesc_t
 *  Param:  input_fptr  file pointer to some primary volume descriptor data
 *  Param:  path        some directory where `p` should be extracted.
 *  Return:  Zero on success, non-zero on failure.
 */
int _i9660pvd_extract (_ISO9660PriVolDesc_t *p, FILE input_fptr[static 1],
                       const char path[static 1]);

#endif /* _ISO_9660_PRIMARY_VOLUME_DESCRIPTOR_DATA_H_ */
