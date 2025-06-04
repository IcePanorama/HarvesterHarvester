/**
 *  main.c - a command-line interface for HarvesterHarvester (HH).
 *
 *  NOTE: This file remains a bit messy. Yes, some of this logic (e.g., the
 *  command-line argument stuff) could easily be abstracted away into its own
 *  file. Given that this stuff is a rather  minor part of the overall program
 *  though--the core HH stuff is the real focus--I think it's probably fine as
 *  is; It's functional, and that's what matters.
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

#define CURR_VERSION ("v2.0.0b")
#define ISSUES_PAGE                                                           \
  ("https://github.com/IcePanorama/HarvesterHarvester/issues")
#define HOME_PAGE ("https://github.com/IcePanorama/HarvesterHarvester/")

static const char def_output_path[] = "output";
static char *output_path = (char *)def_output_path;

static HHOptions_t opts = { 0 };

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

  const char opts_fmt[] = "  %-25s %s\n";
  puts ("Options:");
  printf (opts_fmt, "-h, --help", "Print this help message");
  printf (opts_fmt, "--skip-i9660-dats", "Treat input as internal dat files.");
  printf (opts_fmt, "-o <path>,--output <path>",
          "Set output directory to `<path>` (default: `./output/`)");
  printf (opts_fmt, "--skip-internal-dats",
          "Skip the extraction of internal dat files");
  printf (opts_fmt, "-v, --version", "Print version information");
  putchar ('\n');

  printf ("Report bugs here: <%s>\n", (ISSUES_PAGE));
  printf ("HarvesterHarvester home page: <%s>\n", (HOME_PAGE));
}

static int
handle_args (int argc, char **argv)
{
  int i;
  for (i = 1; i < argc; i++)
    {
      if ((strcmp (argv[i], "-v") == 0)
          || (strcmp (argv[i], "--version") == 0))
        {
          print_ver_info ();
          exit (EXIT_SUCCESS);
        }
      else if ((strcmp (argv[i], "-h") == 0)
               || (strcmp (argv[i], "--help") == 0))
        {
          print_help_info (argv[0]);
          exit (EXIT_SUCCESS);
        }
      else if (strcmp (argv[i], "--skip-i9660-dats") == 0)
        {
          opts.processing_mode = _HHPM_SKIP_I9660_DATS;
        }
      else if ((strcmp (argv[i], "-o") == 0)
               || (strcmp (argv[i], "--output") == 0))
        {
          if (i + 1 >= argc)
            {
              fprintf (stderr,
                       "Improper usage error. Run `%s -h` for proper usage.\n",
                       argv[0]);
              return -1;
            }

          output_path = argv[i + 1];
          i++;
        }
      else if (strcmp (argv[i], "--skip-internal-dats") == 0)
        {
          opts.processing_mode = _HHPM_SKIP_INTERNAL_DATS;
        }
      else // assume remainder of args are paths to dat files
        {
          break;
        }
    }

  if (i != argc) // given list of files
    {
      for (int j = i; j < argc; j++)
        {
          if (hh_extract_filesystem_w_options (argv[j], output_path, &opts)
              != 0)
            return -1;
        }

      exit (EXIT_SUCCESS);
    }

  return 0;
}

static int
batch_process (const char **filenames, const size_t num_files)
{
  for (size_t i = 0; i < num_files; i++)
    {
      if (hh_extract_filesystem_w_options (filenames[i], output_path, &opts)
          != 0)
        return -1;
    }

  return 0;
}

int
main (int argc, char **argv)
{
  opts = HH_DEFAULT_OPTIONS;

  if (argc > 1)
    {
      if (handle_args (argc, argv) != 0)
        return EXIT_FAILURE;
    }

  /*
   *  NOTE: these two arrays are technically duplicated from
   *  `harvester_harvester/known_files.c`, but given that the list of default
   *  files is fixed, it probably doesn't matter.
   */
  const char *i9660_files[3]
      = { "dat-files/HARVEST.DAT", "dat-files/HARVEST3.DAT",
          "dat-files/HARVEST4.DAT" };
  const char *int_files[9]
      = { "DISK1/HARVEST.DAT", "DISK1/HARVEST2.DAT", "DISK1/SOUND.DAT",
          "DISK2/HARVEST.DAT", "DISK2/HARVEST2.DAT", "DISK2/SOUND.DAT",
          "DISK3/HARVEST.DAT", "DISK3/HARVEST2.DAT", "DISK3/SOUND.DAT" };

  switch (opts.processing_mode)
    {
    case _HHPM_SKIP_I9660_DATS:
      if (batch_process (int_files, 9) != 0)
        return EXIT_FAILURE;
      break;
    default:
      if (batch_process (i9660_files, 3) != 0)
        return EXIT_FAILURE;
      break;
    }

  return EXIT_SUCCESS;
}
