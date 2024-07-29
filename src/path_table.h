#ifndef _PATH_TABLE_H_
#define _PATH_TABLE_H_

#include <stdint.h>
#include <stdio.h>

typedef struct path_table_entry
{
  uint8_t directory_identifier_length;
  uint8_t extended_attribute_record_length;
  uint32_t location_of_extent;
  uint16_t parent_directory_number; //!< 1-based index.
  char *directory_identifier;
} path_table_entry;

/** @see  https://wiki.osdev.org/ISO_9660#The_Path_Table */
typedef struct path_table
{
  path_table_entry *entries;
  size_t size;          //!< max size of `entries`.
  size_t current_entry; //!< pointer to most recent item in `entries`.
} path_table;

/**
 *  Initializes a given `path_table`.
 *
 *  @param  pt  path_table to be initialized.
 *  @return zero on success, non-zero on failure.
 *  @see  path_table
 *  @see  destroy_path_table()
 */
int8_t create_path_table (path_table *pt);

/**
 *  Destroys the given `path_table`, freeing any and all memory alloc'd for
 *  `entries`.
 *  @param  pt  path table to be destroyed.
 *  @see  path_table
 *  @see  create_path_table()
 */
void destroy_path_table (path_table *pt);

/**
 *  Prints the given path table out to the stdout, formatting it into a
 *  human-readible form.
 *
 *  @param  pt  path table to which the entry should be added.
 */
void print_path_table (path_table *pt);

/**
 *  Processes the given `path_table` as a Type-L `path_table`, using the data
 *  pointed to by `fptr`.
 *
 *  @param  fptr  data to be processed.
 *  @param  pt    path table to which the data should be processed into.
 *  @see https://wiki.osdev.org/ISO_9660#The_Path_Table
 */
int8_t process_type_l_path_table (FILE *fptr, path_table *pt);

#endif
