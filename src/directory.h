#ifndef _DIRECTORY_H_
#define _DIRECTORY_H_

#include <stdint.h>

typedef struct directory_record
{
  uint8_t length;
  uint8_t extended_attribute_record_length;
  uint32_t location_of_extent;
  uint32_t data_length;
  // recording date & time
  // file flags
  uint8_t file_unit_size; // only used by files recorded in interleaved mode
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

#endif
