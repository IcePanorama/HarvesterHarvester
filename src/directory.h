#ifndef _DIRECTORY_H_
#define _DIRECTORY_H_

/* clang-format off */
#include <stdint.h>
#include <stdlib.h>

#include "datetime.h"
#include "file_flags.h"
/* clang-format on */

typedef struct directory_record
{
  uint8_t record_length;
  uint8_t extended_attribute_record_length;
  uint32_t location_of_extent;
  uint32_t data_length;
  dir_datetime recording_datetime;
  file_flags file_flags;
  uint8_t file_unit_size; // only used by files recorded in interleaved mode
  uint8_t
      interleave_gap_size; // only used by files recorded in interleaved mode
  uint16_t volume_sequence_number;
  uint8_t file_identifier_length;
  char *file_identifier;
} directory_record;

typedef struct directory
{
  directory_record *records;
  size_t size;
  size_t current_record;
} directory;

int8_t create_directory (directory *d);
void destroy_directory (directory *d);
void add_record_to_directory (directory *d, directory_record *r);
void print_directory (directory *d);
void print_directory_record (directory_record *r);

#endif
