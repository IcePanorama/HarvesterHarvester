/**
 *  main.c - a command-line executable for HarvesterHarvester.
 *
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
