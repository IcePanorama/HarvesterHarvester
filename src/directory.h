#ifndef _DIRECTORY_H_
#define _DIRECTORY_H_

#include "datetime.h"
#include "file_flags.h"

#include <stdint.h>
#include <stdlib.h>

/**
 *  A directory record.
 *
 *  @see https://wiki.osdev.org/ISO_9660#Directories
 */
typedef struct directory_record
{
  uint8_t record_length;
  uint8_t extended_attribute_record_length;
  uint32_t location_of_extent;
  uint32_t data_length;
  dir_datetime recording_datetime;
  file_flags file_flags;
  uint8_t file_unit_size; //!< Only used by files recorded in interleaved mode
  uint8_t
      interleave_gap_size; //!< Only used by files recorded in interleaved mode
  uint16_t volume_sequence_number;
  uint8_t file_identifier_length;
  char *file_identifier;
} directory_record;

/**
 *  A collection of directory records which together make a single directory.
 *
 *  @see directory_record
 */
typedef struct directory
{
  directory_record *records;
  size_t size;           //!< the max capacity of `records`.
  size_t current_record; //!< last record in `records`.
} directory;

/**
 *  Initializes a directory with `DIR_STARTING_NUM_RECORDS` records.
 *
 *  @param  d a pointer to a directory
 *  @return zero on success, non-zero on failure.
 *
 *  @see destory_directory()
 */
int8_t create_directory (directory *d);

/**
 *  Destroys all the directory records within a given directory before then
 *  destroying the directory itself.
 *
 *  @see create_directory()
 */
void destroy_directory (directory *d);

/**
 *  Adds a directory record to the given directory, adjusting the size
 *  of said directory's array of records as needed.
 *
 *  @param  d the directory to which the given record should be added.
 *  @param  r the record to add to the aforementioned directory.
 */
void add_record_to_directory (directory *d, directory_record *r);

/**
 *  Prints a given directory to stdout in a human-readable form.
 *
 *  @param  d the directory to be printed.
 *  @see print_directory_record()
 */
void print_directory (directory *d);

/**
 *  Prints a given directory record to the stdout in a human-readable form.
 *
 *  @param  r the directory record to be printed.
 */
void print_directory_record (directory_record *r);

/**
 *  Creates directory records for every sub-directory and file in a given
 *  directory, filling out each data field of said record using the data found
 *  in the file pointed to by `fptr`.
 *
 *  @param  fptr  pointer to the file containing directory data.
 *  @param  d     the directory to which the directory records should be added.
 *  @see  directory
 *  @see  directory_record
 */
void process_directory (FILE *fptr, directory *d);

#endif
