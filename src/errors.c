#include "errors.h"

#include <stdbool.h>
#include <stdlib.h>

const char *CALLOC_FAILED_ERR_MSG_FMT
    = "[HarvesterHarvester]ERROR: unable to calloc string of size %zu.\n";
const char *FOPEN_FAILED_ERR_MSG_FMT
    = "[HarvesterHarvester]ERROR: unable to open output file, %s\n.";

void
improper_usage_error (void)
{
  bool windows;
#ifdef _WIN32
  windows = true;
#else
  windows = false;
#endif
  puts ("[HarvesterHarvester]ERROR: Improper usage!");
  printf ("try: %s HARVEST.DAT\n",
          !windows ? "$ ./HarvesterHarvester" : "HarvesterHarvester.exe");

  exit (1);
}

void
fopen_error (char *filename)
{
  printf ("[HarvesterHarvester]ERROR: Unable to open file, %s.\n", filename);
}

HH_ERRORS
handle_fread_error (size_t actual, size_t expected_bytes)
{
  printf ("[HarvesterHarvester]ERROR: Only read %zu byte(s), expected %zu.\n",
          actual, expected_bytes);
  return HH_FREAD_ERROR;
}

void
handle_unknown_command_line_argument_error (char *arg)
{
  bool windows;
#ifdef _WIN32
  windows = true;
#else
  windows = false;
#endif
  printf ("[HarvesterHarvester]ERROR: unknown command-line argument, %s.\n",
          arg);
  printf ("Run %s --help for a list of command-line arguments.\n",
          !windows ? "./HarvesterHarvester" : "HarvesterHarvester.exe");
  exit (1);
}
