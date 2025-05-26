/**
 *  harvester_harvester/known_files.h - an interface for handling known files
 *  and file associations. Intended for internal usage. All functions are
 *  prefixed by `_hhkf`.
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
#ifndef _HARVESTER_HARVESTER_KNOWN_DAT_FILES_H_
#define _HARVESTER_HARVESTER_KNOWN_DAT_FILES_H_

#include <stdbool.h>

/**
 *  Checks whether the file pointed to by `path` is a known i9660 file system.
 *
 *  Param:  path  A NULL-terminated path to some file.
 *  Returns:  True if it is, otherwise false.
 */
bool _hhkf_is_known_i9660_file (const char path[static 1]);

/**
 *  Checks whether the file pointed to by `path` is a known internal dat file.
 *
 *  Param:  path  A NULL-terminated path to some file.
 *  Returns:  True if it is, otherwise false.
 */
bool _hhkf_is_known_int_dat (const char path[static 1]);

/**
 *  Given the path to an i9660 file system (`path`), returns a list of paths to
 *  its associated internal .dat files. Said list of paths has a length of 3,
 *  and each path in said list is NULL-terminated. If `path` is not a known
 *  i9660 file system, this function prints an error to stderr and returns a
 *  NULL pointer instead.
 *
 *  Param:  path  A NULL-terminated path string to some i9660 file system.
 *  Returns:  List of paths or NULL pointer.
 */
const char **_hhkf_get_int_dat_paths_from_i9660 (const char path[static 1]);

/**
 *  Given the path to an i9660 file system (`path`), returns a list of paths to
 *  its associated internal index files. Said list of paths has a length of 3,
 *  and each path in said list is NULL-terminated. If `path` is not a known
 *  i9660 file system, this function prints an error to stderr and returns a
 *  NULL pointer instead.
 *
 *  Param:  path  A NULL-terminated path string to some i9660 file system.
 *  Returns:  List of paths or NULL pointer.
 */
const char **_hhkf_get_int_idx_paths_from_i9660 (const char path[static 1]);

/**
 *  Given the path to an internal .dat file (`path`), returns a NULL-terminated
 *  path to its associated index file. If `path` is not a known internal .dat
 *  file, this function prints an error to stderr and returns a NULL pointer
 *  instead.
 *
 *  Param:  path  A NULL-terminated path string to some internal .dat file.
 *  Returns:  A NULL-terminated path string to an index file.
 */
const char *_hhkf_get_idx_path_from_int_dat (const char path[static 1]);

#endif /* _HARVESTER_HARVESTER_KNOWN_DAT_FILES_H_ */
