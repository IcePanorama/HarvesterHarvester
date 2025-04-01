#ifndef _ISO_9660_PRIMARY_VOLUME_DESCRIPTOR_H_
#define _ISO_9660_PRIMARY_VOLUME_DESCRIPTOR_H_

#include "dir_rec.h"
#include "path_table_entry.h"
#include "pri_vol_date_time.h"

#include <stdint.h>
#include <stdio.h>

/** See: https://wiki.osdev.org/ISO_9660#The_Primary_Volume_Descriptor */
typedef struct _PriVolDesc_s
{
  char sys_id[32];
  char vol_id[32];

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

  _DirRec_t root_directory_entry;

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

  uint8_t fs_ver; //!< Always `0x01`.
  uint8_t application_used_data[512];

  _PathTableEntry_t *pt_list;
  size_t pt_list_len;      // num of entries stored.
  size_t pt_list_capacity; // max num of entries.
} _PriVolDesc_t;

int _pvd_init (_PriVolDesc_t p[static 1], FILE input_fptr[static 1]);
void _pvd_print (_PriVolDesc_t p[static 1]);
/** Populates path table list and directory record list. */
int _pvd_process (_PriVolDesc_t p[static 1], FILE input_fptr[static 1]);
void _pvd_free (_PriVolDesc_t p[static 1]);

#endif /* _ISO_9660_PRIMARY_VOLUME_DESCRIPTOR_H_ */
