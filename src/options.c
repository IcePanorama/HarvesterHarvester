#include "options.h"
#include "errors.h"
#include "utils.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool OP_BATCH_PROCESS = true;
char *OP_CURRENT_DISK_NAME = NULL;
bool OP_DEBUG_MODE = false;
char *OP_INPUT_DIR = (char *)"dat-files";
char *OP_OUTPUT_DIR = (char *)"output";
char OP_PATH_SEPARATOR = '/';

// TODO: update me when finished
static const char VERSION_NUMBER_STR[9] = "00.00.00";

static void print_out_list_of_command_line_arguments (void);
static void process_flag (char *f);

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
  puts ("Usage: main {options (optional)} {path/to/}[DAT FILE].DAT");
  puts ("Command-line arguments:");
  puts ("\t--debug: enables debug mode, skips extracting large files.");
  puts ("\t--help: displays list of command line arguments.");
  puts ("\t--single: outputs all files into a single directory "
        "(default=output/).");
  puts ("\t--version: prints out the version number.");
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
