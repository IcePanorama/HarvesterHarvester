#include "hh.h"
#include "options.h"

#include <stdlib.h>

int
main (int argc, char **argv)
{
  FILE *fptr = NULL;

  if (argc >= 2)
    handle_command_line_args (argc, argv);

  if (OP_BATCH_PROCESS && !OP_SKIP_DAT_PROCESSING)
    {
      if (hh_batch_process_DAT_files () != 0)
        exit (1);
    }
  else if (!OP_SKIP_DAT_PROCESSING)
    {
      if (hh_setup_extractor (&fptr, argv[argc - 1]) != 0)
        exit (1);

      if (hh_process_DAT_file (fptr) != 0)
        exit (1);

      fclose (fptr);
    }

  if (!OP_SKIP_INT_DAT_PROCESSING && hh_process_internal_dat_files () != 0)
    {
      exit (1);
    }

  return 0;
}
