#include "harvester_harvester/hh.h"
#include "iso_9660/filesystem.h"

#include <stdio.h>

int
hh_extract_filesystem (const char input_path[static 1],
                       const char output_path[static 1])
{
  ISO9660FileSystem_t *fs = i9660_create_fs ();
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

  if (i9660_init_fs (fs, input_file) != 0)
    {
      i9660_free_fs (fs);
      fclose (input_file);
      return -1;
    }

  i9660_print_fs (fs);
  i9660_free_fs (fs);
  fclose (input_file);
  return 0;
  // tmp
  printf ("Unused: %s\n", output_path);
}
