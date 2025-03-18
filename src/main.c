#include "harvester_harvester/hh.h"

#include <stdlib.h>

int
main (void)
{
  const char filename[] = "dat-files/HARVEST.DAT";

  int ret = hh_extract_filesystem (filename, "output");

  return ret == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
