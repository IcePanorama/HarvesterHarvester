#include "index_file.h"
#include "data_reader.h"
#include "log.h"
#include "options.h"
#include "utils.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void
process_index_file (FILE *fptr, index_file *idxf)
{
  index_entry entry;
  entry.entry_start = ftell (fptr);

  fseek (fptr, 0x5, SEEK_CUR); // skip `XFLE#`

  // TODO: REMOVE ME (i->size == 0)
  if (!peek_char_is (fptr, ':') || idxf->size == 0)
    {
      fprintf (
          stderr,
          "ERROR: Unexpected character in index file. Aborting processing.\n");
      return;
    }

  read_string (fptr, entry.full_path, (uint8_t)FULL_PATH_MAX_LEN);

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

  // Need to keep track of which DISK# dir we're in.
  // Not sure how I want to go about doing this atm.
  // prepend_string (entry.full_path, OP_OUTPUT_DIR);

  /**
   *  Every entry is 0x94 bytes apart, the part we're most concerned about is
   *  the last 16 bytes. Currently treating unknown data as junk. If you have
   *  any idea what this data might be used for, please feel free to make
   *  changes.
   */
  fseek (fptr, entry.entry_start + (0x94 - 0x10), SEEK_SET);

  entry.file_start = read_little_endian_data_uint32_t (fptr);
  entry.file_offset = read_little_endian_data_uint32_t (fptr);

  fseek (fptr, 0x4, SEEK_CUR); // Skip zeros

  // Very unnecessary, but we might as well double check our data
  uint32_t value = read_little_endian_data_uint32_t (fptr);
  if (entry.file_offset != value)
    {
      fprintf (stderr, "ERROR: Expected %08X, got %08X.\n", entry.file_offset,
               value);
      return;
    }

  printf ("Entry start: %08X\n", entry.entry_start);
  printf ("Full path: %s\n", entry.full_path);
  printf ("Filename: %s\n", entry.filename);
  printf ("File start: %08X\n", entry.file_start);
  printf ("File offset: %08X\n", entry.file_offset);
}
