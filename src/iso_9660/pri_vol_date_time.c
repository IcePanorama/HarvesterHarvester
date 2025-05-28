/**
 *  Copyright (C) 2024-2025  IcePanorama
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "iso_9660/pri_vol_date_time.h"
#include "iso_9660/binary_reader.h"

#include <stdint.h>
#include <stdlib.h>

struct _ISO9660PVDDateTime_s
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

_ISO9660PVDDateTime_t *
_i9660pvddt_alloc (void)
{
  return calloc (1, sizeof (_ISO9660PVDDateTime_t));
}

void
_i9660pvddt_free (_ISO9660PVDDateTime_t *dt)
{
  if (dt == NULL)
    return;

  free (dt);
}

int
_i9660pvddt_init (_ISO9660PVDDateTime_t *dt, FILE input_fptr[static 1])
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
_i9660pvddt_print (_ISO9660PVDDateTime_t *dt, const char dt_id[static 1])
{
  if (dt == NULL)
    return;

  printf ("%s: %.4s-%.2s-%.2s %.2s:%.2s:%.2s.%.2s (GMT%+02d)\n", dt_id,
          dt->year, dt->month, dt->day, dt->hour, dt->min, dt->sec,
          dt->hundredths_of_sec, (-48 + dt->timezone) >> 2);
}
