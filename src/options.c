#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errors.h"
#include "options.h"

bool debug_mode = false;
char *current_disk_name = NULL;
bool OPT_USE_DEF_INPUT_DIR = true;

// TODO: update me when finished
static const char VERSION_NUMBER_STR[9] = "00.00.00";

static void print_out_list_of_command_line_arguments (void);
static void process_flag (char *f);

/*
 *  handle_command_line_args
 *
 *  TODO: add documentation
 */
void
handle_command_line_args (int argc, char **argv)
{
  int i;
  for (i = 1; i < argc; i++)
    {
      if (argv[i][0] == '-')
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

  char *file = argv[argc - 1];
  size_t len = strlen (file) + 1; // +1 for null terminator
  const char *target = ".DAT";
  const uint8_t target_len = 5;
  if (len > target_len && strcmp (file + (len - target_len), target) == 0)
    {
      // just processing the user-provided file @ the user-provided path
      OPT_USE_DEF_INPUT_DIR = false;
    }
  else
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
      VERSION_NUMBER_STR);
  puts ("Usage: main {options (optional)} {path/to/}[DAT FILE].DAT");
  puts ("Command-line arguments:");
  puts ("\t--debug: enables debug mode, skips extracting large files.");
  puts ("\t--help: displays list of command line arguments.");
  puts ("\t--single: outputs all files into a single directory "
        "(default=output/).");
  puts ("\t--version: prints out the version number.");
}

/*
 *  process_flags
 *
 *  TODO: add documentation
 */
void
process_flag (char *f)
{
  if (strcmp (f, "--debug") == 0)
    {
      debug_mode = true;
    }
  else if (strcmp (f, "--help") == 0)
    {
      print_out_list_of_command_line_arguments ();
      exit (0);
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
