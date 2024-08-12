#include "path_table.h"
#include "data_reader.h"
#include "errors.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static const size_t PT_STARTING_NUM_ENTRIES = 10;
static const size_t PT_GROWTH_RATE = 2;

/**
 *  Resizes a given `path_tables` list of entries by a factor of
 *  `PT_GROWTH_RATE`.
 */
static int8_t resize_path_table_entries (path_table *pt);

/** Prints a path_table_entry to the stdout in a human-readable form. */
static void print_path_table_entry (path_table_entry *e);

/**
 *  Adds a `path_table_entry` to the given `path_table`'s list of `entries`,
 *  resizing it as needed.
 */
static int8_t add_entry_to_path_table (path_table *pt,
                                       path_table_entry *entry);

int8_t
create_path_table (path_table *pt)
{
  pt->entries = malloc (sizeof (path_table_entry) * PT_STARTING_NUM_ENTRIES);
  if (pt->entries == NULL)
    {
      perror ("[HarvesterHarvester]ERROR: Failed to allocate memory for path "
              "table.");
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

int8_t
add_entry_to_path_table (path_table *pt, path_table_entry *entry)
{
  if (pt->current_entry >= pt->size)
    {
      if (resize_path_table_entries (pt) != 0)
        return HH_PT_RESIZE_ERROR;
    }

  pt->entries[pt->current_entry] = *entry;
  pt->current_entry++;
  return 0;
}

int8_t
resize_path_table_entries (path_table *pt)
{
  size_t new_size = (size_t)(pt->size * PT_GROWTH_RATE);
  path_table_entry *new_entries = (path_table_entry *)realloc (
      pt->entries, sizeof (path_table_entry) * new_size);
  if (new_entries == NULL)
    {
      fprintf (stderr,
               "[HarvesterHarvester]ERROR: Ralloc failed for `entries` of "
               "size %zu.\n",
               new_size);
      destroy_path_table (pt);
      return HH_MEM_ALLOC_ERROR;
    }

  pt->entries = new_entries;
  for (size_t i = pt->size; i < new_size; i++)
    {
      pt->entries[i].directory_identifier = NULL;
    }

  pt->size = new_size;
  return 0;
}

void
print_path_table (path_table *pt)
{
  for (size_t i = 0; i < pt->current_entry; i++)
    {
      printf ("Entry %zu:\n", i + 1);
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

int8_t
process_type_l_path_table (FILE *fptr, path_table *pt)
{
  uint8_t dir_identifier_length;
  if (read_single_uint8 (fptr, &dir_identifier_length) != 0)
    return HH_FREAD_ERROR;

  do
    {
      path_table_entry curr;
      curr.directory_identifier_length = dir_identifier_length;

      if (curr.directory_identifier_length != 1)
        curr.directory_identifier_length += 1;

      /* clang-format off */
      if ((read_single_uint8 (fptr, &curr.extended_attribute_record_length) != 0)
          || (read_little_endian_data_uint32_t (fptr, &curr.location_of_extent) != 0)
          || (read_little_endian_data_uint16_t (fptr, &curr.parent_directory_number)) != 0)
        {
          return HH_FREAD_ERROR;
        }
      /* clang-format on */

      curr.directory_identifier
          = (char *)calloc (curr.directory_identifier_length, sizeof (char));

      if (curr.directory_identifier_length != 1)
        {
          size_t bytes_read
              = fread (curr.directory_identifier, sizeof (char),
                       curr.directory_identifier_length - 1, fptr);
          curr.directory_identifier[curr.directory_identifier_length - 1]
              = '\0';
          if (bytes_read
              != sizeof (char) * (curr.directory_identifier_length - 1))
            {
              handle_fread_error (
                  bytes_read,
                  sizeof (char) * (curr.directory_identifier_length - 1));
              return HH_FREAD_ERROR;
            }
        }
      else
        {
          fseek (fptr, 1, SEEK_CUR);
        }

      if (add_entry_to_path_table (pt, &curr) != 0)
        return HH_PT_RESIZE_ERROR;

      if (read_single_uint8 (fptr, &dir_identifier_length) != 0)
        return HH_FREAD_ERROR;

      if (dir_identifier_length == 0) // handle padding field
        {
          if (read_single_uint8 (fptr, &dir_identifier_length) != 0)
            return HH_FREAD_ERROR;
        }
    }
  while (dir_identifier_length != 0);

  return 0;
}
