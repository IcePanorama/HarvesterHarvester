/* clang-format off */
#include <stdio.h>

#include "path_table.h"
/* clang-format on */

static const size_t PATH_TABLE_STARTING_NUM_ENTRIES = 10;

int8_t
create_path_table (path_table *pt)
{
  pt->entries
      = malloc (sizeof (path_table_entry) * PATH_TABLE_STARTING_NUM_ENTRIES);
  if (pt->entries == NULL)
    {
      perror ("ERROR: Failed to allocate memory for path table.");
      return -1;
    }

  pt->size = PATH_TABLE_STARTING_NUM_ENTRIES;
  pt->current_entry = 0;

  for (size_t i = 0; i < PATH_TABLE_STARTING_NUM_ENTRIES; i++)
    {
      pt->entries[i].directory_identifier = NULL;
    }

  return 0;
}

void
destroy_path_table (path_table *pt)
{
  for (size_t i = 0; i < pt->size; i++)
    {
      if (pt->entries[i].directory_identifier == NULL)
        continue;

      free (pt->entries[i].directory_identifier);
      pt->entries[i].directory_identifier = NULL;
    }

  free (pt->entries);
  pt->entries = NULL;
}

void
add_entry_to_path_table (path_table *pt, path_table_entry *entry)
{
  if (pt->current_entry >= pt->size)
    {
      // TODO: handle resizing entries
      free (entry->directory_identifier);
      entry->directory_identifier = NULL;
      return;
    }

  pt->entries[pt->current_entry] = *entry;
  pt->current_entry++;
}
