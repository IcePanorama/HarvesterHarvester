/**
 *  iso_9660/utils.h - utility functions for processing and extracting ISO 9660
 *  file systems. Intended for internal usage. All public functions are
 *  prefixed by `_i9660u`.
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
#ifndef _ISO_9660_UTILITIES_H_
#define _ISO_9660_UTILITIES_H_

#include <stddef.h>
#include <stdint.h>

/**
 *  Prepends `str` of length `str_len` with `prefix` of length `prefix_len`.
 *  Assumes `str` is large enough to accomidate `prefix`.
 *
 *  Returns: zero on success, non-zero on failure.
 */
int _i9660u_prepend_str (char str[static 1], const size_t str_len,
                         const char prefix[static 1], const size_t prefix_len);

/**
 *  Writes `data_size` bytes of `data` to file at `path`.
 *
 *  Returns: Zero on success, non-zero on failure.
 */
int _i9660u_export_data (const uint8_t data[static 1], const size_t data_size,
                         const char path[static 1]);

#endif /* _ISO_9660_UTILITIES_H_ */
