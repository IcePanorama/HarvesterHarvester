#ifndef _ISO_9660_PRIMARY_VOLUME_DESCRIPTOR_DATE_TIME_H_
#define _ISO_9660_PRIMARY_VOLUME_DESCRIPTOR_DATE_TIME_H_

#include <stdint.h>
#include <stdio.h>

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

int _pvddt_init (_PVDDateTime_t dt[static 1], FILE input_fptr[static 1]);
void _pvddt_print (_PVDDateTime_t dt[static 1], const char *dt_id);

#endif /* _ISO_9660_PRIMARY_VOLUME_DESCRIPTOR_DATE_TIME_H_ */
