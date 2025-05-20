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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CURR_VERSION ("v2.0.0")
#define ISSUES_PAGE                                                           \
  ("https://github.com/IcePanorama/HarvesterHarvester/issues")
#define HOME_PAGE ("https://github.com/IcePanorama/HarvesterHarvester/")

static const char def_output_path[] = "output";
static char *output_path = (char *)def_output_path;

static void
print_ver_info (void)
{
  printf ("HarvesterHarvester %s\n", (CURR_VERSION));

  // Copyright boilerplate.
  puts ("Copyright (C) 2024-2025 IcePanorama");
  puts ("License GPLv3+: GNU GPL version 3 or later "
        "<https://gnu.org/licenses/gpl.html>");
  puts ("This is free software: you are free to change and redistribute it.");
  puts ("There is NO WARRANTY, to the extent permitted by law.");
}

static void
print_help_info (const char *exe_name)
{
  puts ("Usage:");
  printf ("  %s [options] [path/to/file.dat ...]\n", exe_name);
  putchar ('\n');

  puts ("Options:");
  puts ("  -h, --help\tPrint this help message");
  puts ("  -v, --version\tPrint version information");
  putchar ('\n');

  printf ("Report bugs here: <%s>\n", (ISSUES_PAGE));
  printf ("HarvesterHarvester home page: <%s>\n", (HOME_PAGE));
}

static int
handle_args (int argc, char **argv)
{
  for (int i = 1; i < argc; i++)
    {
      if ((strcmp (argv[i], "-v") == 0)
          || (strcmp (argv[i], "--version") == 0))
        {
          print_ver_info ();
          return 0;
        }
      else if ((strcmp (argv[i], "-h") == 0)
               || (strcmp (argv[i], "--help") == 0))
        {
          print_help_info (argv[0]);
          return 0;
        }
      else // assume remainder of args are paths to dat files
        {
          for (int j = i; j < argc; j++)
            {
              if (hh_extract_filesystem (argv[j], output_path) != 0)
                return -1;
            }
        }
    }

  return 0;
}

int
main (int argc, char **argv)
{
  if (argc > 1)
    {
      if (handle_args (argc, argv) != 0)
        return EXIT_FAILURE;

      return EXIT_SUCCESS;
    }

  const char *filename[3]
      = { "dat-files/HARVEST.DAT", "dat-files/HARVEST3.DAT",
          "dat-files/HARVEST4.DAT" };

  if ((hh_extract_filesystem (filename[0], output_path) != 0)
      || (hh_extract_filesystem (filename[1], output_path) != 0)
      || (hh_extract_filesystem (filename[2], output_path) != 0))
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
