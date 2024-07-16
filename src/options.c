#include <string.h>

#include "errors.h"
#include "options.h"

bool debug_mode = false;
bool separate_outputs = false;

/*
 *  handle_command_line_args
 *
 *  TODO: add documentation
 */
void
handle_command_line_args (int argc, char **argv)
{
  for (int i = 2; i < argc; i++)
    {
      if (strcmp (argv[i], "--debug") == 0)
        {
          debug_mode = true;
        }
      else if (strcmp (argv[i], "--separate") == 0)
        {
          separate_outputs = true;
        }
      else if (strcmp (argv[i], "--help") == 0)
        {
          // TODO: print out a list of possible command-line args
        }
      else
        {
          handle_unknown_command_line_argument_error (argv[i]);
        }
    }
}
