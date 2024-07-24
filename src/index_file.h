/**
 *  index_file.h - typedef for processing Harvester's `INDEX.00#` files.
 */
#ifndef _INDEX_FILE_H_
#define _INDEX_FILE_H_

#include <stdint.h>
#include <stdio.h>

/** Both of these are probably too large, but better to be safe than sorry. */
#define FULL_PATH_MAX_LEN 256
#define FILENAME_MAX_LEN 32

typedef struct index_entry
{
  /*
    Probably gonna ignore the XFLE# part, making the following comment
    inaccurate.  TODO: update later.
  */
  char full_path[FULL_PATH_MAX_LEN]; //!< File path beginning w/ `XFLE#:\`
  char filename[FILENAME_MAX_LEN]; //!< Name of the actual file (w/ extension)
  uint32_t file_start;
  uint32_t file_offset;
} index_entry;

typedef struct index_file
{
  index_entry *indicies;
  size_t size;          //!< max size of `indicies`.
  size_t current_index; //!< pointer to last item in `indicies`.
} index_file;

int8_t create_index_file (index_file *i);
int8_t destroy_index_file (index_file *i);

#endif
