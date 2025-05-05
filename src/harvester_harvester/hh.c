#include "harvester_harvester/hh.h"
#include "harvester_harvester/idx_file.h"
#include "iso_9660.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// TODO: these defaults should be loaded from some config file.
static const char *known_i9660_fs[3]
    = { "dat-files/HARVEST.DAT", "dat-files/HARVEST3.DAT",
        "dat-files/HARVEST4.DAT" };

static bool
is_known_i9660_file (const char *path)
{
  for (size_t i = 0; i < 3; i++)
    {
      if (strcmp (known_i9660_fs[i], path) == 0)
        return true;
    }

  return false;
}

static int
extract_i9660_fs (const char *input_path, const char *output_path)
{
  ISO9660FileSystem_t *fs = i9660_alloc ();
  if (fs == NULL)
    {
      fprintf (stderr, "Out of memory error.\n");
      return -1;
    }

  FILE *input_file = fopen (input_path, "rb");
  if (input_file == NULL)
    {
      fprintf (stderr, "Error opening file %s.\n", input_path);
      return -1;
    }

  if ((i9660_init (fs, input_file) != 0)
      || (i9660_extract (fs, input_file, output_path) != 0))
    {
      i9660_free (fs);
      fclose (input_file);
      return -1;
    }

  i9660_free (fs);
  fclose (input_file);
  return 0;
}

static int
extract_int_dat_fs (const char *dat_path, const char *idx_path)
{
  _IndexFile_t *idx = _idx_alloc ();
  if (idx == NULL)
    {
      fprintf (stderr, "%s: Out of memory error.\n", __func__);
      return -1;
    }

  if (_idx_init (idx, idx_path) != 0)
    {
      _idx_free (idx);
      return -1;
    }

  _idx_free (idx);
  return 0;
  printf ("%s %s\n", dat_path, idx_path);
}

int
hh_extract_filesystem (const char input_path[static 1],
                       const char output_path[static 1])
{
  if (is_known_i9660_file (input_path))
    {
      if (extract_i9660_fs (input_path, output_path) != 0)
        return -1;

      // TODO: for both of these: load from file/build dynamically.
      const char *internal_dat_files[3]
          = { "output/DISK1/HARVEST.DAT", "output/DISK1/HARVEST2.DAT",
              "output/DISK1/SOUND.DAT" };
      const char *assoc_idx_files[3]
          = { "output/DISK1/INDEX.001", "output/DISK1/INDEX.003",
              "output/DISK1/INDEX.002" };

      for (size_t i = 0; i < 3; i++)
        {
          if (extract_int_dat_fs (internal_dat_files[i], assoc_idx_files[i])
              != 0)
            return -1;
          break;
        }
    }
  else
    {
      fprintf (stderr,
               "Error: Unrecognized ISO 9660 filesystem, or currently "
               "unsupported file type: %s.\n",
               input_path);
      return -1;
    }

  return 0;
}
