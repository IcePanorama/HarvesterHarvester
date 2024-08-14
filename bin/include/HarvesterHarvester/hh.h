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
/**
 *  hh.h - the main interface to interact with HarvesterHarvester
 */
#ifndef _HARVESTER_HARVESTER_H_
#define _HARVESTER_HARVESTER_H_

#include <stdint.h>
#include <stdio.h>

/**
 *  Creates a `FILE *` for the given `filename`, handling error messages as
 *  needed, while also checking the end-user's operating system and updating
 *  `OP_PATH_SEPARATOR`. The caller is responsible for closing the file
 *  pointer when they're finished with it.
 *
 *  @param  fptr  a file pointer to the file associated with the given filename
 *  @param  filename  the path to a DAT file
 *  @return zero on success, non-zero on failure.
 */
int8_t hh_setup_extractor (FILE **fptr, char *filename);

/**
 *  Processes a DAT file pointed to by the given `FILE *`.
 *  <p>This is accomplished by first processing the volume descriptor header
 *  (checking that this is indeed a primary volume descriptor after it does so)
 *  and the volume descriptor data. Once complete, it updates the
 *  `CURRENT_DISK_NAME` variable using the appropriate data. Finally, it
 *  constructs a path table, a extracts every file from the given DAT file to
 *  the `OP_OUTPUT_DIR`.
 *
 *  @param  fptr  a file pointer to a DAT file.
 *  @see  https://wiki.osdev.org/ISO_9660
 *  @see  process_volume_descriptor_header()
 *  @see  process_volume_descriptor_data()
 *  @see  create_path_table()
 *  @see  process_type_l_path_table()
 *  @see  extract_directory()
 *  @return zero on success, non-zero on failure.
 */
int8_t hh_process_DAT_file (FILE *fptr);

/**
 *  Processes all the `.DAT` files found in `OP_INPUT_DIR`, one at a time.
 *
 *  @return zero on success, non-zero on failure.
 */
int8_t hh_batch_process_DAT_files (void);

/**
 *  Process all the new `.DAT` files created by `process_DAT_file`. These
 *  `.DAT` files are not ISO 9660 file systems, rather they're some proprietary
 *  format. These files can be processed by reading the associated `INDEX.00#`
 *  file, which contains all the filenames, paths, and location of data for
 *  files stored in these new `.DAT` files.
 *
 *  @return int8_t zero on success, non-zero on failure.
 */
int8_t hh_process_internal_dat_files (void);

#endif
