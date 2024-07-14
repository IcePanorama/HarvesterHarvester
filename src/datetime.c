/* clang-format off */
#include <stdio.h>

#include "datetime.h"
/* clang-format on */

void
print_dec_datetime (dec_datetime dt)
{
  printf ("%s-%s-%s @ %s:%s:%s.%s (%02X)\n", dt.year, dt.month, dt.day,
          dt.hour, dt.minute, dt.second, dt.hundredths_of_a_second,
          dt.time_zone_offset);
}
