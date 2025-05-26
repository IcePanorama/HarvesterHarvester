/**
 *  harvester_harvester/hh.h - an interface for extracting data from the .dat
 *  files of DigiFX Interactive's 1996 game, Harvester.
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
#ifndef _HARVESTER_HARVESTER_H_
#define _HARVESTER_HARVESTER_H_

#include "harvester_harvester/options.h"

extern const HHOptions_t HH_DEFAULT_OPTIONS;

/**
 *  Extracts the given i9660 or Harvester file system (`input_path`) to the
 *  provided `output_path` using `opts`.
 *
 *  param:  input_path   path to some dat file.
 *  param:  output_path  directory where extracted data should be placed.
 *  param:  opts         customized options for extraction
 *  returns: zero on success, non-zero on failure.
 */
int hh_extract_filesystem_w_options (const char input_path[static 1],
                                     const char output_path[static 1],
                                     const HHOptions_t opts);

/**
 *  Extracts the given i9660 or Harvester file system (`input_path`) to the
 *  provided `output_path` using `HH_DEFAULT_OPTIONS`. Internally, this just
 *  calls `hh_extract_filesystem_w_options`.
 *
 *  param:  input_path   path to some dat file.
 *  param:  output_path  directory where extracted data should be placed.
 *  returns: zero on success, non-zero on failure.
 */
int hh_extract_filesystem (const char input_path[static 1],
                           const char output_path[static 1]);

#endif /* _HARVESTER_HARVESTER_H_ */
