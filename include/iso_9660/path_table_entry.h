#ifndef _ISO_9660_PATH_TABLE_ENTRY_H_
#define _ISO_9660_PATH_TABLE_ENTRY_H_

#include <stdint.h>
#include <stdio.h>

typedef struct _PathTableEntry_s
{
  uint8_t dir_id_len;
  uint8_t extended_attribute_record_len;
  uint32_t extent_loc;
  uint16_t parent_dir_num;
  char dir_id[UINT8_MAX]; // See `dir_id_len`.
} _PathTableEntry_t;

int _pte_init (_PathTableEntry_t p[static 1], FILE input_fptr[static 1]);
void _pte_print (_PathTableEntry_t p[static 1]);
int _pte_extract (_PathTableEntry_t p[static 1], uint16_t lb_size,
                  FILE input_fptr[static 1]);

#endif /* _ISO_9660_PATH_TABLE_ENTRY_H_ */
