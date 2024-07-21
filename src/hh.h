#ifndef _HARVESTER_HARVESTER_H_
#define _HARVESTER_HARVESTER_H_

#include <stdio.h>

/**
 *  Creates a `FILE *` for the given `filename`, handling error messages as
 *  needed, while also checking the end-user's operating system and updating
 *  `OP_PATH_SEPARATOR`. The caller is responsible for closing the file
 *  pointer when they're finished with it.
 *
 *  @param  filename `char *` of the path to a DAT file
 *  @return a pointer to the given file, if it exist.
 */
static FILE *setup_extractor (char *filename);

/**
 *  Processes a DAT file pointed to by the given `FILE *`.
 *  <p>This is accomplished by first processing the volume descriptor header
 *  (checking that this is indeed a primary volume descriptor after it does so)
 *  and the volume descriptor data. Once complete, it updates the
 *  `OP_CURRENT_DISK_NAME` variable using the appropriate data. Finally, it
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
 */
static void process_DAT_file (FILE *fptr);

/**
 *  Processes all the `.DAT` files found in `OP_INPUT_DIR`, one at a time.
 */
static int batch_process_DAT_files (void);

#endif
