#ifndef _ISO_9660_PATH_TABLE_ENTRY_H_
#define _ISO_9660_PATH_TABLE_ENTRY_H_

#include <stdint.h>

typedef struct _PathTableEntry_s
{
  uint8_t dir_id_len;
  uint8_t extended_attribute_record_len;
  uint32_t extent_loc;
  uint16_t parent_dir_num;
  char dir_id[UINT8_MAX]; // See `dir_id_len`.
} _PathTableEntry_t;

#endif /* _ISO_9660_PATH_TABLE_ENTRY_H_ */
