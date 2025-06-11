/**
 *  iso_9660/pri_vol_date_time.h - an interface for processing primary volume
 *  descriptor date/time data. Intended for internal usage. All public
 *  functions are prefixed by `_i9660pvddt`.
 *
 *  FIXME: Currently not doing anything with this data. Technically could be
 *  used for file metadata.
 *
 *  See: https://wiki.osdev.org/ISO_9660#Date/time_format
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
#ifndef _ISO_9660_PRIMARY_VOLUME_DESCRIPTOR_DATE_TIME_H_
#define _ISO_9660_PRIMARY_VOLUME_DESCRIPTOR_DATE_TIME_H_

#include <stdio.h>

typedef struct _ISO9660PVDDateTime_s _ISO9660PVDDateTime_t;

/**
 *  Allocates an _ISO9660PVDDateTime_t. Said primary volume date/time data must
 *  later be freed via `_i9660pvddt_free`. Return value may be NULL upon calloc
 *  failure.
 *
 *  Return:  pointer to allocated i9660 pri vol descriptor date time data
 *  See:  `_i9660pvddt_free`
 */
_ISO9660PVDDateTime_t *_i9660pvddt_alloc (void);

/**
 *  Frees memory allocated by `dt`. Returns immediately if `dt` is NULL.
 *
 *  Param:  dt  a pointer to some _ISO9660PVDDateTime_t
 */
void _i9660pvddt_free (_ISO9660PVDDateTime_t *dt);

/**
 *  Prints all the data fields of `dt` to stdout. Returns immediately if `dt`
 *  is NULL.
 *
 *  Param:  dt  some _ISO9660PVDDateTime_t
 */
void _i9660pvddt_print (_ISO9660PVDDateTime_t *dt, const char dt_id[static 1]);

/**
 *  Initializes all the data fields of `dt` using the data provided via
 *  `input_fptr`. Returns non-zero if `dt` is NULL.
 *
 *  Param:  dt          some _ISO9660PVDDateTime_t
 *  Param:  input_fptr  file pointer to some pri vol desc date time data
 *  Return:  Zero on success, non-zero on failure.
 */
int _i9660pvddt_init (_ISO9660PVDDateTime_t *dt, FILE input_fptr[static 1]);

#endif /* _ISO_9660_PRIMARY_VOLUME_DESCRIPTOR_DATE_TIME_H_ */
