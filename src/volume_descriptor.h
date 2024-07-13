#ifndef _VOLUME_DESCRIPTOR_H_
#define _VOLUME_DESCRIPTOR_H_

#include <stdint.h>
#include <string.h>

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
