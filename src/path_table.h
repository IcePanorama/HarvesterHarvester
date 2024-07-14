#ifndef _PATH_TABLE_H_
#define _PATH_TABLE_H_

#include <stdint.h>
#include <stdlib.h>

// See: https://wiki.osdev.org/ISO_9660#The_Path_Table
typedef struct path_table_entry
{
  uint8_t directory_identifier_length;
  uint8_t extended_attribute_record_length;
  uint32_t location_of_extent;
  uint16_t parent_directory_number;
  char *directory_identifier;
} path_table_entry;

typedef struct path_table
{
  path_table_entry *entries;
  size_t size;
} path_table;

#endif
