#include "harvester_harvester/hh.h"

#include <stdlib.h>

int
main (void)
{
  const char *filename[3]
      = { "dat-files/HARVEST.DAT", "dat-files/HARVEST3.DAT",
          "dat-files/HARVEST4.DAT" };
  const char output_path[] = "output";

  if ((hh_extract_filesystem (filename[0], output_path) != 0)
      || (hh_extract_filesystem (filename[1], output_path) != 0)
      || (hh_extract_filesystem (filename[2], output_path) != 0))
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
