/**
 *  options.h - container for all of hh's options.
 *
 *  Currently also home to some global variables until I figure out a better
 *  place for them to be stored.
 */
//  Copyright (C) 2024  IcePanorama
//  This file is a part of HarvesterHarvester.
//  HarvesterHarvester is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by the
//  Free Software Foundation, either version 3 of the License, or (at your
//  option) any later version.
//  This program is distributed in the hope that it will be useful, but WITHOUT
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//  more details.
//
//  You should have received a copy of the GNU General Public License along
//  with this program.  If not, see <https://www.gnu.org/licenses/>.
#ifndef _HH_OPTIONS_H_
#define _HH_OPTIONS_H_

#include <stdbool.h>
#include <stdint.h>

/** If true, batch process all the files in `OP_INPUT_DIR`. */
extern bool OP_BATCH_PROCESS;

/**
 *  If true, skip `.DAT` file processing and go directly to processing
 *  `INDEX.00#` files.
 */
extern bool OP_SKIP_DAT_PROCESSING;

/** Skips files larger than `OP_DEBUG_FILE_SIZE_LIMIT` if true. */
extern bool OP_DEBUG_MODE;

/** Minimum size of files to be skipped if `OP_DEBUG_MODE` is true. */
extern uint32_t OP_DEBUG_FILE_SIZE_LIMIT;

/**
 *  Directory where hh will try to batch process files from if
 *  `OP_BATCH_PROCESS` is true. Default is `./dat-files/`.
 */
extern char *OP_INPUT_DIR;

/**
 *  Directory where hh will try to output extracted files to. Default is
 *  `./output/`.
 */
extern char *OP_OUTPUT_DIR;

/**
 *  Character used to separate directory/file names in a path string. Depends
 *  on the user's operating system. `\\` for Windows, `/` for *nix.
 */
extern char OP_PATH_SEPARATOR;

/**
 *  Current disk name as denoted by the `volume_identifier` in
 *  `volume_descriptor_data`. Not really an option, should be moved elsewhere.
 *  Stored here for the sake of convenience.
 *
 *  @see volume_descriptor_data
 */
extern char *CURRENT_DISK_NAME;

/** Processes command-line arguments, setting options as needed. */
void handle_command_line_args (int argc, char **argv);

#endif
