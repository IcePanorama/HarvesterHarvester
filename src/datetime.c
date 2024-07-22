#include "datetime.h"

#include <stdio.h>

void
print_dec_datetime (dec_datetime dt)
{
  printf ("%s-%s-%s @ %s:%s:%s.%s (%02X)\n", dt.year, dt.month, dt.day,
          dt.hour, dt.minute, dt.second, dt.hundredths_of_a_second,
          dt.time_zone_offset);
}

void
print_dir_datetime (dir_datetime dt)
{
  printf ("%02X-%02X-%02X @ %02X:%02X:%02X (%02X)\n", dt.year, dt.month,
          dt.day, dt.hour, dt.minute, dt.second, dt.time_zone_offset);
}
