#ifndef _ISO_9660_PRIMARY_VOLUME_DESCRIPTOR_H_
#define _ISO_9660_PRIMARY_VOLUME_DESCRIPTOR_H_

#include "ISO9660/directory_record.h"

#include <stdint.h>

typedef struct ISO9660PrimaryVolumeDateTime_s
{
  char year[4];
  char month[2];
  char day[2];
  char hour[2];
  char minute[2];
  char second[2];
  char hundredths_of_a_second[2];
  /** "offset from GMT in 15 minute intervals, [from] -48 ... to 52." */
  uint8_t timezone;
} ISO9660PrimaryVolumeDateTime_t;

/** @see: https://wiki.osdev.org/ISO_9660#The_Primary_Volume_Descriptor */
typedef struct ISO9660PrimaryVolumeDescriptorData_s
{
  char system_identifier[32];
  char volume_identifier[32];

  /** "number of logical blocks in which the volume is recorded." */
  uint32_t volume_space_size;
  uint16_t volume_set_size;        //!< "number of disks."
  uint16_t volume_sequence_number; //!< number of this disk in the set.
  uint16_t logical_block_size;
  uint32_t path_table_size;
  uint32_t type_l_path_table_location;
  uint32_t optional_type_l_path_table_location;
  uint32_t type_m_path_table_location;
  uint32_t optional_type_m_path_table_location;

  ISO9660DirectoryRecord_t root_directory_entry;

  char volume_set_identifier[128];
  char publisher_identifier[128];
  char data_preparer_identifier[128];
  char application_identifier[128];
  char copyright_file_identifier[37];
  char abstract_file_identifier[37];
  char bibliographic_file_identifier[37];

  ISO9660PrimaryVolumeDateTime_t volume_creation_date_time;
  ISO9660PrimaryVolumeDateTime_t volume_modification_date_time;
  ISO9660PrimaryVolumeDateTime_t volume_expiration_date_time;
  ISO9660PrimaryVolumeDateTime_t volume_effective_date_time;

  uint8_t file_structure_version; //!< Always `0x01`.
  uint8_t application_used_data[512];
} ISO9660PrimaryVolumeDescriptorData_t;


#endif /* _ISO_9660_PRIMARY_VOLUME_DESCRIPTOR_H_ */
