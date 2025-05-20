#include "harvester_harvester/known_files.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: these defaults could be loaded from some config file.
static const char *known_i9660_fs[3]
    = { "dat-files/HARVEST.DAT", "dat-files/HARVEST3.DAT",
        "dat-files/HARVEST4.DAT" };

struct IntFileAssoc_s
{
  const char *dat_files[3];
  const char *idx_files[3];
  /* clang-format off */
} int_assocs[] = {
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

bool
_hhkf_is_known_i9660_file (const char path[static 1])
{
  for (size_t i = 0; i < 3; i++)
    {
      if (strcmp (known_i9660_fs[i], path) == 0)
        return true;
    }

  return false;
}

const char **
_hhkf_get_i9660_int_dat_paths (const char path[static 1])
{
  for (size_t i = 0; i < 3; i++)
    {
      if (strcmp (known_i9660_fs[i], path) == 0)
        {
          return int_assocs[i].dat_files;
        }
    }

  fprintf (stderr, "%s: Unknown ISO 9660 filesystem, %s.\n", __func__, path);
  return NULL;
}

const char **
_hhkf_get_i9660_int_idx_paths (const char path[static 1])
{
  for (size_t i = 0; i < 3; i++)
    {
      if (strcmp (known_i9660_fs[i], path) == 0)
        {
          return int_assocs[i].idx_files;
        }
    }

  fprintf (stderr, "%s: Unknown ISO 9660 filesystem, %s.\n", __func__, path);
  return NULL;
}
