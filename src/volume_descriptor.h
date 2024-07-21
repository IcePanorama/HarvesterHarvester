#ifndef _VOLUME_DESCRIPTOR_H_
#define _VOLUME_DESCRIPTOR_H_

#include "datetime.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define SYSTEM_IDENTIFIER_LEN 33
#define VOLUME_IDENTIFIER_LEN 33
#define ROOT_DIRECTORY_ENTRY_LEN 34
#define VOLUME_SET_IDENTIFIER_LEN 129
#define PUBLISHER_IDENTIFIER_LEN 129
#define DATA_PREPARER_IDENTIFIER_LEN 129
#define APPLICATION_IDENTIFIER_LEN 129
#define COPYRIGHT_FILE_IDENTIFIER_LEN 38
#define ABSTRACT_FILE_IDENTIFIER_LEN 38
#define BIBLIOGRAPHIC_FILE_IDENTIFIER_LEN 38
#define DAT_FILE_CREATION_SOFTWARE_IDENTIFIER_LEN 9
#define DAT_FILE_CREATION_SOFTWARE_VERSION_NUMBER_LEN 11

// See: https://wiki.osdev.org/ISO_9660#The_Primary_Volume_Descriptor
typedef struct volume_descriptor_data
{
  char system_identifier[SYSTEM_IDENTIFIER_LEN];
  char volume_identifier[VOLUME_IDENTIFIER_LEN];
  uint32_t volume_space_size;
  uint16_t volume_set_size;
  uint16_t volume_sequence_number;
  uint16_t logical_block_size;
  uint32_t path_table_size;
  uint32_t type_l_path_table_location;
  uint32_t optional_type_l_path_table_location;
  uint32_t type_m_path_table_location;
  uint32_t optional_type_m_path_table_location;
  uint8_t root_directory_entry[ROOT_DIRECTORY_ENTRY_LEN];
  char volume_set_identifier[VOLUME_SET_IDENTIFIER_LEN];
  char publisher_identifier[PUBLISHER_IDENTIFIER_LEN];
  char data_preparer_identifier[DATA_PREPARER_IDENTIFIER_LEN];
  char application_identifier[APPLICATION_IDENTIFIER_LEN];
  char copyright_file_identifier[COPYRIGHT_FILE_IDENTIFIER_LEN];
  char abstract_file_identifier[ABSTRACT_FILE_IDENTIFIER_LEN];
  char bibliographic_file_identifier[BIBLIOGRAPHIC_FILE_IDENTIFIER_LEN];
  dec_datetime volume_creation_date_and_time;
  dec_datetime volume_modification_date_and_time;
  dec_datetime volume_expiration_date_and_time;
  dec_datetime volume_effective_date_and_time;
  uint8_t file_structure_version; // always `0x01`
  char DAT_file_creation_software_identifier
      [DAT_FILE_CREATION_SOFTWARE_IDENTIFIER_LEN];
  char DAT_file_creation_software_version_number
      [DAT_FILE_CREATION_SOFTWARE_VERSION_NUMBER_LEN];
} volume_descriptor_data;

// See: https://wiki.osdev.org/ISO_9660#Volume_Descriptors
typedef struct volume_descriptor
{
  uint8_t type_code;
  char identifier[6]; // always `CD001`
  uint8_t version;
  volume_descriptor_data data;
} volume_descriptor;

void create_volume_descriptor (volume_descriptor *vd, uint8_t type,
                               uint8_t version);
void print_volume_descriptor_header (volume_descriptor *vd);
void print_volume_descriptor_data (volume_descriptor_data *vdd);
void process_volume_descriptor_header (FILE *fptr, volume_descriptor *vd);
void process_volume_descriptor_data (FILE *fptr, volume_descriptor_data *vdd);

#endif
