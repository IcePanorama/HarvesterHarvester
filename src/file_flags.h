#ifndef _FILE_FLAGS_H_
#define _FILE_FLAGS_H_

#include <stdbool.h>
#include <stdio.h>

/**
 *  The file flags for a directory record.
 *
 *  @see https://wiki.osdev.org/ISO_9660#Directories
 *  @see directory_record
 */
typedef struct file_flags
{
  bool hidden;
  bool subdirectory;
  bool associated_file; //!< No clue what this means
  bool extended_attribute_record_contains_owner_and_group_permissions;
  /**
   *  If false, this directory record is not the file's final resting place;
   *  the file spans over multiple extents.
   */
  bool final_directory_record;
} file_flags;

/**
 *  Creates an instance of `file_flags`, initializing all values to their
 *  defaults.
 *
 *  @return initialized `file_flags` instance.
 *  @see file_flags
 */
file_flags create_file_flags (void);

/**
 *  Outputs a given instance of `file_flags` to stdout in a human-readable
 *  form.
 *
 *  @param  ff  the instance of `file_flags` to be printed.
 *  @see  file_flags
 */
void print_file_flags (file_flags *ff);

#endif
