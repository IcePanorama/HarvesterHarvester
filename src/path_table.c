#include "path_table.h"

#include <stdio.h>
#include <stdlib.h>

static const size_t PT_STARTING_NUM_ENTRIES = 10;
static const size_t PT_GROWTH_RATE = 2; // doubles in size every time it grows.
void resize_path_table_entries (path_table *pt);
void print_path_table_entry (path_table_entry *e);

int8_t
create_path_table (path_table *pt)
{
  pt->entries = malloc (sizeof (path_table_entry) * PT_STARTING_NUM_ENTRIES);
  if (pt->entries == NULL)
    {
      perror ("ERROR: Failed to allocate memory for path table.");
      return -1;
    }

  pt->size = PT_STARTING_NUM_ENTRIES;
  pt->current_entry = 0;

  for (size_t i = 0; i < PT_STARTING_NUM_ENTRIES; i++)
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
      resize_path_table_entries (pt);
    }

  pt->entries[pt->current_entry] = *entry;
  pt->current_entry++;
}

void
resize_path_table_entries (path_table *pt)
{
  size_t new_size = (size_t)(pt->size * PT_GROWTH_RATE);
  path_table_entry *new_entries = (path_table_entry *)realloc (
      pt->entries, sizeof (path_table_entry) * new_size);
  if (new_entries == NULL)
    {
      perror ("ERROR: failed to reallocate memory for the path table "
              "entries.");

      destroy_path_table (pt);
      exit (1);
    }

  pt->entries = new_entries;
  for (size_t i = pt->size; i < new_size; i++)
    {
      pt->entries[i].directory_identifier = NULL;
    }

  pt->size = new_size;
}

void
print_path_table (path_table *pt)
{
  for (size_t i = 0; i < pt->current_entry; i++)
    {
      printf ("Entry %ld:\n", i + 1);
      print_path_table_entry (&pt->entries[i]);
    }
}

void
print_path_table_entry (path_table_entry *e)
{
  printf ("\tDirectory identifier length: %02X\n",
          e->directory_identifier_length);
  printf ("\tExtended attribute record length: %02X\n",
          e->extended_attribute_record_length);
  printf ("\tLocation of extant: %08X\n", e->location_of_extent);
  printf ("\tParent directory number: %04X\n", e->parent_directory_number);
  printf ("\tDirectory identifier: %s\n", e->directory_identifier);
}
