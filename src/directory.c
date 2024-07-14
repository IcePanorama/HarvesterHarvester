/* clang-format off */
#include <stdio.h>
#include <stdlib.h>

#include "directory.h"
/* clang-format on */

static const size_t DIR_STARTING_NUM_RECORDS = 10;

int8_t
create_directory (directory *d)
{
  d->records = malloc (sizeof (directory_record) * DIR_STARTING_NUM_RECORDS);
  if (d->records == NULL)
    {
      perror ("ERROR: Failed to allocate memory for directory.");
      return -1;
    }

  d->size = DIR_STARTING_NUM_RECORDS;
  d->current_record = 0;

  for (size_t i = 0; i < DIR_STARTING_NUM_RECORDS; i++)
    {
      d->records[i].file_identifier = NULL;
    }

  return 0;
}

void
destroy_directory (directory *d)
{
  for (size_t i = 0; i < d->size; i++)
    {
      if (d->records[i].file_identifier == NULL)
        continue;

      free (d->records[i].file_identifier);
      d->records[i].file_identifier = NULL;
    }

  free (d->records);
  d->records = NULL;
}

void
add_record_to_directory (directory *d, directory_record *r)
{
  if (d->current_record >= d->size)
    {
      // TODO: handle expanding records.
      return;
    }

  d->records[d->current_record] = *r;
  d->current_record++;
}
