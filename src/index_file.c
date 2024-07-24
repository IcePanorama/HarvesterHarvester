#include "index_file.h"

#include <stdio.h>
#include <stdlib.h>

static const size_t IDX_STARTING_NUM_INDICIES = 10;
// static const size_t IDX_RECORDS_GROWTH_RATE = 2;

int8_t
create_index_file (index_file *i)
{
  i->indicies = malloc (IDX_STARTING_NUM_INDICIES * sizeof (index_entry));
  if (i->indicies == NULL)
    {
      perror ("ERROR: unable to malloc array of indicies.");
      return -1;
    }

  i->size = IDX_STARTING_NUM_INDICIES;
  i->current_index = 0;

  return 0;
}

int8_t
destroy_index_file (index_file *i)
{
  free (i->indicies);
  return 0;
}
