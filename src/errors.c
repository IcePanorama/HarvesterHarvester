#include "errors.h"
#include "log.h"

#include <stdbool.h>
#include <stdlib.h>

const char *CALLOC_FAILED_ERR_MSG_FMT = "Unable to calloc string of size %zu.";
const char *FOPEN_FAILED_ERR_MSG_FMT = "Unable to open output file, %s.";

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

HH_ERRORS
handle_calloc_error (size_t size)
{
  hh_log (HH_LOG_ERROR, CALLOC_FAILED_ERR_MSG_FMT, size);
  return HH_MEM_ALLOC_ERROR;
}
