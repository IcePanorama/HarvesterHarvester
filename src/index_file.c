#include "index_file.h"
#include "data_reader.h"
#include "errors.h"
#include "options.h"
#include "utils.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const size_t IDX_STARTING_NUM_INDICIES = 10;
static const size_t IDX_RECORDS_GROWTH_RATE = 2;

int8_t
create_index_file (index_file *i)
{
  i->indicies = malloc (IDX_STARTING_NUM_INDICIES * sizeof (index_entry));
  if (i->indicies == NULL)
    {
      fprintf (stderr, "ERROR: unable to malloc array of %zu indicies.",
               IDX_STARTING_NUM_INDICIES * sizeof (index_entry));
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

int8_t
process_index_file (FILE *fptr, index_file *idxf)
{
  while (!peek_eof (fptr))
    {
      index_entry entry;
      entry.entry_start = ftell (fptr);

      fseek (fptr, 0x5, SEEK_CUR); // skip `XFLE#`

      if (!peek_char_is (fptr, ':'))
        {
          fprintf (stderr, "ERROR: Unexpected character in index file. "
                           "Aborting processing.\n");
          return -1;
        }

      if (read_string (fptr, entry.full_path, (uint8_t)FULL_PATH_MAX_LEN) != 0)
        return HH_FREAD_ERROR;

      uint8_t len = strlen (entry.full_path);
      char *last_word = entry.full_path;
      for (uint8_t i = 0; i < len; i++)
        {
          if (entry.full_path[i] == '\\')
            {
              last_word = entry.full_path + i + 1;
              entry.full_path[i] = OP_PATH_SEPARATOR;
            }
        }
      strcpy (entry.filename, last_word);

      /**
       *  Every entry is 0x94 bytes apart, the part we're most concerned about
       *  is the last 16 bytes. Currently treating unknown data in between as
       *  junk. If you have any idea what this data might be used for, please
       *  feel free to make changes.
       */
      fseek (fptr, entry.entry_start + (0x94 - 0x10), SEEK_SET);

      /* clang-format off */
      if ((read_little_endian_data_uint32_t (fptr, &entry.file_start) != 0)
          || (read_little_endian_data_uint32_t (fptr, &entry.file_offset) != 0))
        {
          return HH_FREAD_ERROR;
        }
      /* clang-format on */

      fseek (fptr, 0x4, SEEK_CUR); // Skip zeros

      // Very unnecessary, but we might as well double check our data
      uint32_t value;
      if (read_little_endian_data_uint32_t (fptr, &value) != 0)
        return HH_FREAD_ERROR;

      if (entry.file_offset != value)
        {
          fprintf (stderr, "ERROR: Expected %08X, got %08X.\n",
                   entry.file_offset, value);
          return -1;
        }

      print_index_entry (&entry);
      if (add_entry_to_index_file (idxf, &entry) != 0)
        {
          return -1;
        }
    }

  return 0;
}

void
print_index_entry (index_entry *idxe)
{
  printf ("Entry start: %08X\n", idxe->entry_start);
  printf ("Full path: %s\n", idxe->full_path);
  printf ("Filename: %s\n", idxe->filename);
  printf ("File start: %08X\n", idxe->file_start);
  printf ("File offset: %08X\n", idxe->file_offset);
}

int8_t
add_entry_to_index_file (index_file *file, index_entry *entry)
{
  if (file->current_index >= file->size)
    {
      if (resize_indicies (file) != 0)
        {
          return -1;
        }
    }

  file->indicies[file->current_index] = *entry;
  file->current_index++;
  return 0;
}

int8_t
resize_indicies (index_file *idx)
{
  size_t new_size = idx->size * IDX_RECORDS_GROWTH_RATE;
  index_entry *new_indicies = (index_entry *)realloc (
      idx->indicies, new_size * sizeof (index_entry));
  if (new_indicies == NULL)
    {
      fprintf (stderr, "ERROR: unable to ralloc index from size %zu to %zu.\n",
               idx->size, new_size);
      destroy_index_file (idx);
      return -1;
    }

  idx->indicies = new_indicies;
  idx->size = new_size;
  return 0;
}
