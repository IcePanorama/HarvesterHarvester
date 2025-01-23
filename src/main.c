#include "iso_9660.h"

#include <stdint.h>
#include <stdio.h>

static int8_t open_file (FILE **fptr, const char *filename);

int
main (void)
{
  const char *filename = "dat-files/HARVEST.DAT";

  FILE *input_file;
  if (open_file (&input_file, filename) != 0)
    return -1;

  Iso9660FileSystem_t fs;
  if (create_iso_9660_filesystem_from_file (input_file, &fs) != 0)
    {
      fclose (input_file);
      return -1;
    }

  fclose (input_file);
  return 0;
}

int8_t
open_file (FILE **fptr, const char *filename)
{
  *fptr = fopen (filename, "rb");
  if (*fptr == NULL)
    {
      fprintf (stderr, "ERROR: Unable to open file, %s.\n", filename);
      return -1;
    }

  return 0;
}
