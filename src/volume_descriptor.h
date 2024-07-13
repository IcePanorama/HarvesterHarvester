#ifndef _VOLUME_DESCRIPTOR_H_
#define _VOLUME_DESCRIPTOR_H_

/* clang-format off */
#include <stdint.h>
#include <string.h>

#include "dec_datetime.h"
/* clang-format on */

// See: https://wiki.osdev.org/ISO_9660#The_Primary_Volume_Descriptor
typedef struct volume_descriptor_data {
  char system_identifier[33];
  char volume_identifier[33];
  uint32_t volume_space_size;
  uint16_t volume_set_size;
  uint16_t volume_sequence_number;
  uint16_t logical_block_size;
  uint32_t path_table_size;
  uint32_t type_l_path_table_location;
  uint32_t optional_type_l_path_table_location;
  uint32_t type_m_path_table_location;
  uint32_t optional_type_m_path_table_location;
  uint8_t root_directory_entry[34];
  char volume_set_identifier[129];
  char publisher_identifier[129];
  char data_preparer_identifier[129];
  char application_identifier[129];
  char copyright_file_identifier[38];
  char abstract_file_header[38];
  char bibliographic_file_identifier[38];
  dec_datetime volume_creation_date_and_time;
  dec_datetime volume_modification_date_and_time;
  dec_datetime volume_expiration_date_and_time;
  dec_datetime volume_effective_date_and_time;
  uint8_t file_structure_version;                   // always `0x01`
} volume_descriptor_data;

// See: https://wiki.osdev.org/ISO_9660#Volume_Descriptors
typedef struct volume_descriptor {
  uint8_t type_code;
  char identifier[6]; // always `CD001`
  uint8_t version;
  uint8_t data[2041];
} volume_descriptor;

void create_volume_descriptor(volume_descriptor* vd, uint8_t type, uint8_t version);
void print_volume_descriptor_header (volume_descriptor *vd);

#endif
