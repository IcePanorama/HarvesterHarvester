#include "iso_9660/pri_vol_date_time.h"
#include "iso_9660/binary_reader.h"

#include <stdint.h>
#include <stdlib.h>

/** See: https://wiki.osdev.org/ISO_9660#Date/time_format. */
struct _PVDDateTime_s
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
};

_PVDDateTime_t *
_pvddt_alloc (void)
{
  return calloc (1, sizeof (_PVDDateTime_t));
}

void
_pvddt_free (_PVDDateTime_t *dt)
{
  if (dt == NULL)
    return;

  free (dt);
}

int
_pvddt_init (_PVDDateTime_t *dt, FILE input_fptr[static 1])
{
  if (dt == NULL)
    return -1;

  if ((_i9660br_read_str (input_fptr, dt->year, 4) != 0)
      || (_i9660br_read_str (input_fptr, dt->month, 2) != 0)
      || (_i9660br_read_str (input_fptr, dt->day, 2) != 0)
      || (_i9660br_read_str (input_fptr, dt->hour, 2) != 0)
      || (_i9660br_read_str (input_fptr, dt->min, 2) != 0)
      || (_i9660br_read_str (input_fptr, dt->sec, 2) != 0)
      || (_i9660br_read_str (input_fptr, dt->hundredths_of_sec, 2) != 0)
      || (_i9660br_read_u8 (input_fptr, &dt->timezone) != 0))
    {
      return -1;
    }

  return 0;
}

void
_pvddt_print (_PVDDateTime_t *dt, const char dt_id[static 1])
{
  if (dt == NULL)
    return;

  printf ("%s: %.4s-%.2s-%.2s %.2s:%.2s:%.2s.%.2s (GMT%+02d)\n", dt_id,
          dt->year, dt->month, dt->day, dt->hour, dt->min, dt->sec,
          dt->hundredths_of_sec, (-48 + dt->timezone) >> 2);
}
