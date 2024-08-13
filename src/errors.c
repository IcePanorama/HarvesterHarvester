#include "errors.h"
#include "log.h"

#include <stdbool.h>
#include <stdlib.h>

const char *CALLOC_FAILED_ERR_MSG_FMT
    = "Unable to calloc string of size %zu.\n";
const char *FOPEN_FAILED_ERR_MSG_FMT = "Unable to open output file, %s\n.";

void
improper_usage_error (void)
{
  bool windows;
#ifdef _WIN32
  windows = true;
#else
  windows = false;
#endif
  hh_log (HH_LOG_ERROR, "Improper usage!\ntry: %s HARVEST.DAT\n",
          !windows ? "$ ./HarvesterHarvester" : "HarvesterHarvester.exe");
  exit (1);
}

void
fopen_error (char *filename)
{
  hh_log (HH_LOG_ERROR, "Unable to open file, %s.", filename);
}

HH_ERRORS
handle_fread_error (size_t actual, size_t expected_bytes)
{
  hh_log (HH_LOG_ERROR, "Only read %zu byte(s), expected %zu.", actual,
          expected_bytes);
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
  hh_log (HH_LOG_ERROR,
          "Unknown command-line argument, %s. Run %s --help for a list of "
          "command-line arguments.",
          arg, !windows ? "./HarvesterHarvester" : "HarvesterHarvester.exe");
  exit (1);
}
