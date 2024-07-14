#ifndef _FILE_FLAGS_H_
#define _FILE_FLAGS_H_

#include <stdbool.h>
#include <stdio.h>

typedef struct file_flags
{
  bool hidden;
  bool subdirectory;
  bool associated_file; // No clue what this means
  bool extended_attribute_record_contains_owner_and_group_permissions;
  bool final_directory_record; // if false, this is not the final directory
                               // record; file spans over multiple extents
} file_flags;

file_flags create_file_flags (void);
void print_file_flags (file_flags *ff);

#endif
