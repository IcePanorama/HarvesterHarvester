#ifndef _ISO_9660_PATH_TABLE_ENTRY_H_
#define _ISO_9660_PATH_TABLE_ENTRY_H_

#include <stdint.h>
#include <stdio.h>

typedef struct ISO9660PathTableEntry_s
{
  uint8_t directory_identifier_length;
  uint8_t extended_attribute_record_length;
  uint32_t extent_location;
  uint16_t parent_directory_number;
  char directory_identifier[UINT8_MAX];
} ISO9660PathTableEntry_t;

int alloc_pt_entries_array (ISO9660PathTableEntry_t **pts, size_t size);
int read_path_tables_from_file (FILE input_fptr[static 1],
                                ISO9660PathTableEntry_t *pt_list[static 1],
                                size_t ptable_entries_len[static 1],
                                uint32_t pt_start_loc, uint32_t pt_size);

/**
 *  Creates a path string for every entry in a given path table.
 *  @returns zero on success, non-zero on failure.
 */
int build_paths_from_pt_list (ISO9660PathTableEntry_t pt_list[static 1],
                              char *path_list[static 1], size_t list_len,
                              const char pvd_vol_id[static 1],
                              const char output_dir[static 1]);

#endif /* _ISO_9660_PATH_TABLE_ENTRY_H_ */
