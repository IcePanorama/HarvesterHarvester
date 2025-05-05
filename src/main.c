#include "harvester_harvester/hh.h"

#include <stdlib.h>

int
main (void)
{
  const char *filename[3]
      = { "dat-files/HARVEST.DAT", "dat-files/HARVEST3.DAT",
          "dat-files/HARVEST4.DAT" };
  const char output_path[] = "output";

  int ret = (hh_extract_filesystem (filename[0], output_path));
  // tmp
  /*
ret += (hh_extract_filesystem (filename[1], output_path));
ret += (hh_extract_filesystem (filename[2], output_path));
  */

  return ret == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
