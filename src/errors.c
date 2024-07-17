/* clang-format off */
#include <stdbool.h>
#include <stdlib.h>

#include "errors.h"
/* clang-format on */

void
improper_usage_error (void)
{
  bool windows;
#ifdef __WIN32
  windows = true;
#else
  windows = false;
#endif
  puts ("ERROR: Improper usage!");
  printf ("try: %s HARVEST.DAT\n", !windows ? "$ ./main" : "main.exe");

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
  bool windows;
#ifdef __WIN32
  windows = true;
#else
  windows = false;
#endif
  printf ("ERROR: unknown command-line argument, %s.\n", arg);
  printf ("Run %s --help for a list of command-line arguments.\n",
          !windows ? "./main" : "main.exe");
  exit (1);
}
