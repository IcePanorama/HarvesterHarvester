//  Copyright (C) 2024  IcePanorama
//  This file is a part of HarvesterHarvester.
//  HarvesterHarvester is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by the
//  Free Software Foundation, either version 3 of the License, or (at your
//  option) any later version.
//  This program is distributed in the hope that it will be useful, but WITHOUT
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//  more details.
//
//  You should have received a copy of the GNU General Public License along
//  with this program.  If not, see <https://www.gnu.org/licenses/>.
#include "options.h"
#include "errors.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Option defaults */
bool OP_BATCH_PROCESS = true;
bool OP_SKIP_DAT_PROCESSING = false;
bool OP_SKIP_INT_DAT_PROCESSING = false;
bool OP_DEBUG_MODE = false;
char *OP_INPUT_DIR = (char *)"dat-files";
char *OP_OUTPUT_DIR = (char *)"output";
char OP_PATH_SEPARATOR = '/';
uint32_t OP_DEBUG_FILE_SIZE_LIMIT = 0xF00000; //!< 0xF00000 = 15 MiB

/* Global defaults */
char *CURRENT_DISK_NAME = NULL;

static const char VERSION_NUMBER_STR[9] = "01.00.00";

/** Prints a list of supported command-line arugments to stdout. */
static void print_out_list_of_command_line_arguments (void);

/** Processes options beginning with `-` character, called flags. */
static void process_flag (char *f);

void
handle_command_line_args (int argc, char **argv)
{
  int i;
  for (i = 1; i < argc; i++)
    {
      /** Handle multi-part command-line arguments. */
      if (strncmp (argv[i], "-d", 2) == 0)
        {
          i++;
          size_t path_len = strlen (argv[i]);
          if (argv[i][path_len - 1] == '/' || argv[i][path_len - 1] == '\\')
            {
              argv[i][path_len - 1] = '\0';
            }
          OP_INPUT_DIR = argv[i];
        }
      else if (strncmp (argv[i], "-o", 2) == 0)
        {
          i++;
          size_t path_len = strlen (argv[i]);
          if (argv[i][path_len - 1] == '/' || argv[i][path_len - 1] == '\\')
            {
              argv[i][path_len - 1] = '\0';
            }
          OP_OUTPUT_DIR = argv[i];
        }
      else if (argv[i][0] == '-')
        {
          process_flag (argv[i]);
        }
      else
        {
          break;
        }
    }

  if (i == argc)
    return;

  if (is_string_dat_file (argv[argc - 1]))
    {
      // just processing the user-provided file @ the user-provided path
      OP_BATCH_PROCESS = false;
    }
  else
    {
      handle_unknown_command_line_argument_error (argv[argc - 1]);
    }
}

void
print_out_list_of_command_line_arguments (void)
{
  printf (
      "HarvesterHarvester (HH) Version %s Copyright (C) 2024 IcePanorama\n",
      VERSION_NUMBER_STR);
#ifdef _WIN32
  puts ("Usage:\n\tHarvesterHarvester_[ARCH].exe [options] "
        "[path_to_dat_file]\n");
#else
  puts ("Usage:\n\t./HarvesterHarvester [options] [path/to/dat_file.dat]\n");
#endif
  puts ("Command-line arguments:");
  puts ("\t--debug: enables debug mode, skips extracting large files.");
  puts ("\t--help: displays list of command line arguments.");
  puts ("\t--version: prints out the version number.");
  //  TODO: have some usage.md file that explains this in more detail.
  //  add a note here saying to read that file for details.
  puts ("\t-d [path/to/some-dir]: specify the input directory for batch "
        "processing dat files.");
  puts ("\t-e: don't extract internal dat files.");
  puts ("\t-i: extract internal dat files only.");
  puts ("\t-o [path/to/some-dir]: extract dat files to the given directory.");
  puts ("\nOptional:");
  puts ("\tpath/to/dat_file.dat: specify the path to a dat file to be "
        "processed.");
}

void
process_flag (char *f)
{
  if (strcmp (f, "--debug") == 0)
    {
      OP_DEBUG_MODE = true;
    }
  else if (strcmp (f, "--help") == 0)
    {
      print_out_list_of_command_line_arguments ();
      exit (0);
    }
  else if (strcmp (f, "-e") == 0)
    {
      OP_SKIP_INT_DAT_PROCESSING = true;
    }
  else if (strcmp (f, "-i") == 0)
    {
      OP_SKIP_DAT_PROCESSING = true;
    }
  else if (strcmp (f, "--version") == 0)
    {
      printf ("HarvesterHarvester v%s\n", VERSION_NUMBER_STR);
      exit (0);
    }
  else
    {
      handle_unknown_command_line_argument_error (f);
    }
}
