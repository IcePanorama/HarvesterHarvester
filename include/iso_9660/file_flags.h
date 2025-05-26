/**
 *  iso_9660/file_flags.h - the file flags of a directory record. Intended for
 *  internal usage. All public defines are prefixed by `_I9660FF`.
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
#ifndef _ISO_9660_DIRECTORY_RECORD_FILE_FLAGS_H_
#define _ISO_9660_DIRECTORY_RECORD_FILE_FLAGS_H_

#include <stdint.h>

typedef uint8_t _FileFlags_t;

/**
 *  File flag bits.
 *  See: https://wiki.osdev.org/ISO_9660#Directories.
 */
#define _I9660FF_HIDDEN_FILE_BIT (0)
#define _I9660FF_IS_DIRECTORY_BIT (1)
#define _I9660FF_IS_ASSOC_FILE_BIT (2)
#define _I9660FF_FMT_IN_EAR_BIT (3) // EAR: Extended attribute record
#define _I9660FF_PERMS_IN_EAR_BIT (4)
// Bits 5 and 6 are reserved.
#define _I9660FF_NOT_FINAL_DIR_BIT (7)

#endif /* _ISO_9660_DIRECTORY_RECORD_FILE_FLAGS_H_ */
