#include "hh.h"

int
main (void)
{
  const char *filename = "dat-files/HARVEST.DAT";

  if (hh_extract_filesystem (filename, "output") != 0)
    return -1;

  return 0;
}
