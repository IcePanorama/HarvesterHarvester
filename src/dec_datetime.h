#ifndef _DEC_DATETIME_H_
#define _DEC_DATETIME_H_

// See: https://wiki.osdev.org/ISO_9660#Date/time_format
typedef struct dec_datetime
{
  char year[5];
  char month[3];
  char day[3];
  char hour[3];
  char minute[3];
  char second[3];
  char hundredths_of_a_second[3];
  uint8_t time_zone_offset;
} dec_datetime;

#endif
