#include "iso_9660/pri_vol_date_time.h"
#include "iso_9660/binary_reader.h"

int
_pvddt_init (_PVDDateTime_t dt[static 1], FILE input_fptr[static 1])
{
  if ((_br_read_str (input_fptr, dt->year, 4) != 0)
      || (_br_read_str (input_fptr, dt->month, 2) != 0)
      || (_br_read_str (input_fptr, dt->day, 2) != 0)
      || (_br_read_str (input_fptr, dt->hour, 2) != 0)
      || (_br_read_str (input_fptr, dt->min, 2) != 0)
      || (_br_read_str (input_fptr, dt->sec, 2) != 0)
      || (_br_read_str (input_fptr, dt->hundredths_of_sec, 2) != 0)
      || (_br_read_u8 (input_fptr, &dt->timezone) != 0))
    {
      return -1;
    }

  return 0;
}

void
_pvddt_print (_PVDDateTime_t dt[static 1], const char *dt_id)
{
  printf ("%s: %.4s-%.2s-%.2s %.2s:%.2s:%.2s.%.2s (GMT%+02d)\n", dt_id,
          dt->year, dt->month, dt->day, dt->hour, dt->min, dt->sec,
          dt->hundredths_of_sec, (-48 + dt->timezone) >> 2);
}
