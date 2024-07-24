#include "index_file.h"
#include "data_reader.h"
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
  fseek (fptr, 0x5, SEEK_SET); // skip `XFLE#`
  // TODO: REMOVE ME (i->size == 0)
  if (!peek_char_is (fptr, ':') || idxf->size == 0)
    {
      fprintf (
          stderr,
          "ERROR: Unexpected character in index file. Aborting processing.\n");
      return;
    }

  index_entry entry;
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

  printf ("Full path: %s\n", entry.full_path);
  printf ("Filename: %s\n", entry.filename);
}
