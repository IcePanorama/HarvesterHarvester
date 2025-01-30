#include "hh.h"
#include "iso_9660.h"

#include <stdio.h>

static int8_t open_file (FILE **fptr, const char *filename);

int8_t
hh_extract_filesystem (const char input_file_path[static 1],
                       const char output_dir_path[static 1])
{
  FILE *input_file;
  if (open_file (&input_file, input_file_path) != 0)
    return -1;

  // tmp
  puts (output_dir_path);

  Iso9660FileSystem_t fs;
  if (iso_9660_create_filesystem_from_file (input_file, &fs) != 0)
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
