#ifndef _ISO_9660_PATH_TABLE_ENTRY_H_
#define _ISO_9660_PATH_TABLE_ENTRY_H_

#include <stdint.h>

typedef struct ISO9660PathTableEntry_s
{
  uint8_t directory_identifier_length;
  uint8_t extended_attribute_record_length;
  uint32_t extent_location;
  uint16_t parent_directory_number;
  char directory_identifier[UINT8_MAX];
} ISO9660PathTableEntry_t;

#endif /* _ISO_9660_PATH_TABLE_ENTRY_H_ */
