#ifndef _ISO_9660_PRIMARY_VOLUME_DESCRIPTOR_H_
#define _ISO_9660_PRIMARY_VOLUME_DESCRIPTOR_H_

#include <stdint.h>

/** See: https://wiki.osdev.org/ISO_9660#Date/time_format. */
typedef struct _PVDDateTime_s
{
  char year[4];
  char month[2];
  char day[2];
  char hour[2];
  char min[2];
  char sec[2];
  char hundredths_of_sec[2];
  /** "offset from GMT in 15 minute intervals, [from] -48 ... to 52." */
  uint8_t timezone;
} _PVDDateTime_t;

/** See: https://wiki.osdev.org/ISO_9660#The_Primary_Volume_Descriptor */
typedef struct _PriVolDesc_s
{
  char system_id[32];
  char volume_id[32];

  /** "number of logical blocks in which the volume is recorded." */
  uint32_t vol_space_size;
  uint16_t vol_set_size; // "number of disks."
  uint16_t vol_seq_num;  // number of this disk in the set.
  uint16_t logical_blk_size;
  uint32_t path_table_size;
  uint32_t type_l_path_table_loc;
  uint32_t optional_type_l_path_table_loc;
  uint32_t type_m_path_table_loc;
  uint32_t optional_type_m_path_table_loc;

  /*
  ISO9660DirectoryRecord_t root_directory_entry;
  */

  char vol_set_id[128];
  char publisher_id[128];
  char data_preparer_id[128];
  char application_id[128];
  char copyright_file_id[37];
  char abstract_file_id[37];
  char bibliographic_file_id[37];

  _PVDDateTime_t creation_date_time;
  _PVDDateTime_t modification_date_time;
  _PVDDateTime_t expiration_date_time;
  _PVDDateTime_t effective_date_time;

  uint8_t fs_version; //!< Always `0x01`.
  uint8_t application_used_data[512];
} _PriVolDesc_t;

#endif /* _ISO_9660_PRIMARY_VOLUME_DESCRIPTOR_H_ */
