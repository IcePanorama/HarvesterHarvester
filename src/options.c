#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errors.h"
#include "options.h"

// TODO: update me when finished
static char version_number_str[9] = "00.00.00";

bool debug_mode = false;
bool separate_outputs = false;

void print_out_list_of_command_line_arguments (void);

/*
 *  handle_command_line_args
 *
 *  TODO: add documentation
 */
void
handle_command_line_args (int argc, char **argv)
{
  for (int i = 1; i < argc; i++)
    {
      if (strcmp (argv[i], "--debug") == 0)
        {
          debug_mode = true;
        }
      else if (strcmp (argv[i], "--single") == 0)
        {
          separate_outputs = true;
        }
      else if (strcmp (argv[i], "--help") == 0)
        {
          print_out_list_of_command_line_arguments ();
          exit (0);
        }
      else if (strcmp (argv[i], "--version") == 0)
        {
          printf ("HarvesterHarvester v%s\n", version_number_str);
          exit (0);
        }
      else if (argv[i][0] == '-')
        {
          handle_unknown_command_line_argument_error (argv[i]);
        }
      else
        {
          break;
        }
    }

  char *file = argv[argc - 1];
  size_t len = strlen (file);
  const char *target = ".DAT";
  const uint8_t target_len = 5;
  if (len <= target_len && strcmp (file + (len - target_len), target) != 0)
    {
      handle_unknown_command_line_argument_error (argv[argc - 1]);
    }
}

/*
 *  print_out_list_of_command_line_arguments
 *
 *  TODO: add documentation
 */
void
print_out_list_of_command_line_arguments (void)
{
  printf (
      "HarvesterHarvester (HH) Version %s Copyright (C) 2024 IcePanorama\n",
      version_number_str);
  puts ("Usage: main {path/to/}[DAT FILE].dat {options (optional)}");
  puts ("Command-line arguments:");
  puts ("\t--debug: enables debug mode, skips extracting large files.");
  puts ("\t--help: displays list of command line arguments.");
  puts ("\t--single: outputs all files into a single directory "
        "(default=output/).");
  puts ("\t--version: prints out the version number.");
}
