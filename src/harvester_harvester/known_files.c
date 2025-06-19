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
#include "harvester_harvester/known_files.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 *  Yes, these defaults could be loaded from some config file, however, they're
 *  also unchanging, at least for our use cases. Should users express interest
 *  in being able to customize these in the future, then we'll worry about all
 *  that.
 */
static const char *known_i9660_fs[3]
    = { "HARVEST.DAT", "HARVEST3.DAT", "HARVEST4.DAT" };

struct IntFileAssoc_s
{
  const char *dat_files[3];
  const char *idx_files[3];
  /* clang-format off */
} int_assocs[] =
{
  {
    .dat_files = { "DISK1/HARVEST.DAT", "DISK1/HARVEST2.DAT", "DISK1/SOUND.DAT" },
    .idx_files = { "DISK1/INDEX.001", "DISK1/INDEX.003", "DISK1/INDEX.002" }
  },
  {
    .dat_files = { "DISK2/HARVEST.DAT", "DISK2/HARVEST2.DAT", "DISK2/SOUND.DAT" },
    .idx_files = { "DISK2/INDEX.001", "DISK2/INDEX.003", "DISK2/INDEX.002" }
  },
  {
    .dat_files = { "DISK3/HARVEST.DAT", "DISK3/HARVEST2.DAT", "DISK3/SOUND.DAT" },
    .idx_files = { "DISK3/INDEX.001", "DISK3/INDEX.003", "DISK3/INDEX.002" }
  }
};
/* clang-format on */

/**
 *  Compares the end of `a` to `b`. Param `a_len` is just there to let me cache
 *  that value when doing repeated function calls (instead of calculating
 *  strlen each time).
 *  FIXME: should be case insensitive.
 */
static bool
strequ_from_end (const char *a, const size_t a_len, const char *b)
{
  const size_t b_len = strlen (b);
  if (a_len > b_len)
    {
      const char *a_end = a + (a_len - b_len);
      return strcmp (b, a_end) == 0;
    }
  else
    return strcmp (b, a) == 0;
}

static bool
str_found_in_arr (const char *str, const char **arr)
{
  const size_t str_len = strlen (str);
  for (size_t i = 0; i < 3; i++)
    {
      if (strequ_from_end (str, str_len, arr[i]))
        return true;
    }

  return false;
}

bool
_hhkf_is_known_i9660_file (const char path[static 1])
{
  return str_found_in_arr (path, known_i9660_fs);
}

bool
_hhkf_is_known_int_dat (const char path[static 1])
{
  for (size_t i = 0; i < 3; i++)
    {
      if (str_found_in_arr (path, int_assocs[i].dat_files))
        return true;
    }

  return false;
}

const char **
_hhkf_get_int_dat_paths_from_i9660 (const char path[static 1])
{
  const size_t path_len = strlen (path);
  for (size_t i = 0; i < 3; i++)
    {
      if (strequ_from_end (path, path_len, known_i9660_fs[i]))
        return int_assocs[i].dat_files;
    }

  fprintf (stderr, "%s: Unknown ISO 9660 filesystem, %s.\n", __func__, path);
  return NULL;
}

const char **
_hhkf_get_int_idx_paths_from_i9660 (const char path[static 1])
{
  const size_t path_len = strlen (path);
  for (size_t i = 0; i < 3; i++)
    {
      if (strequ_from_end (path, path_len, known_i9660_fs[i]))
        return int_assocs[i].idx_files;
    }

  fprintf (stderr, "%s: Unknown ISO 9660 filesystem, %s.\n", __func__, path);
  return NULL;
}

const char *
_hhkf_get_idx_path_from_int_dat (const char path[static 1])
{
  const size_t path_len = strlen (path);
  for (size_t i = 0; i < 3; i++)
    {
      for (size_t j = 0; j < 3; j++)
        {
          if (strequ_from_end (path, path_len, int_assocs[i].dat_files[j]))
            return int_assocs[i].idx_files[j];
        }
    }

  fprintf (stderr, "%s: Unknown internal dat file, %s.\n", __func__, path);
  return NULL;
}
