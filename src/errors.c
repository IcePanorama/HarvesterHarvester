/* clang-format off */
#include <stdlib.h>

#include "errors.h"
/* clang-format on */

void
improper_usage_error (void)
{
  const char *EXECUTABLE_NAME = "main";

  puts ("ERROR: Improper usage!");
  printf ("try: $./%s HARVEST.DAT\n", EXECUTABLE_NAME);

  exit (1);
}

void
fopen_error (char *filename)
{
  printf ("ERROR: Unable to open file, %s.\n", filename);
  exit (1);
}

void
handle_fread_error (FILE *fptr, size_t actual, size_t expected_bytes)
{
  printf ("ERROR: Only read %ld byte(s), expected %ld.\n", actual,
          expected_bytes);
  fclose (fptr);
  exit (1);
}

/*
 *  handle_unknown_command_line_argument_error
 *
 *  TODO: add documentation.
 */
void
handle_unknown_command_line_argument_error (char *arg)
{
  printf ("ERROR: unknown command-line argument, %s.\n", arg);
  exit (1);
}
