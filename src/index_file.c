//  Copyright (C) 2024  IcePanorama
//  This file is a part of HarvesterHarvester.
//  HarvesterHarvester is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by the
//  Free Software Foundation, either version 3 of the License, or (at your
//  option) any later version.
//  This program is distributed in the hope that it will be useful, but WITHOUT
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//  more details.
//
//  You should have received a copy of the GNU General Public License along
//  with this program.  If not, see <https://www.gnu.org/licenses/>.
#include "index_file.h"
#include "data_reader.h"
#include "errors.h"
#include "log.h"
#include "options.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>

static const size_t IDX_STARTING_NUM_INDICIES = 10;
static const size_t IDX_RECORDS_GROWTH_RATE = 2;

/**
 *  Grows a given `index_file`'s `entries` attribute by
 *  `IDX_RECORDS_GROWTH_RATE`.
 */
static int8_t resize_indicies (index_file *idx);

int8_t
create_index_file (index_file *i)
{
  i->indicies = malloc (IDX_STARTING_NUM_INDICIES * sizeof (index_entry));
  if (i->indicies == NULL)
    {
      return handle_malloc_error ("index file indicies.");
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
          hh_log (HH_LOG_ERROR,
                  "Unexpected character in index file. Aborting processing.");
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
      if ((read_little_endian_data_uint32_t (fptr, &entry.start) != 0)
          || (read_little_endian_data_uint32_t (fptr, &entry.size) != 0))
        {
          return HH_FREAD_ERROR;
        }
      /* clang-format on */

      fseek (fptr, 0x4, SEEK_CUR); // Skip zeros

      // Very unnecessary, but we might as well double check our data
      uint32_t value;
      if (read_little_endian_data_uint32_t (fptr, &value) != 0)
        return HH_FREAD_ERROR;

      if (entry.size != value)
        {
          hh_log (HH_LOG_ERROR, "Expected 0x%08X, got 0x%08X.", entry.size,
                  value);
          return -1;
        }

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
  printf ("File start: %08X\n", idxe->start);
  printf ("File size: %08X\n", idxe->size);
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
      destroy_index_file (idx);
      return handle_realloc_error ("indicies", idx->size, new_size);
    }

  idx->indicies = new_indicies;
  idx->size = new_size;
  return 0;
}
