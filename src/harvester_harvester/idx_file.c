#include "harvester_harvester/idx_file.h"
#include "harvester_harvester/binary_reader.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/**
 *  Each entry is 94h bytes. Each entry contains `XFLE#:`, followed by a NULL-
 *  terminated path string, and finally 10h bytes of data. `MAX_PATH_LEN` is
 *  calculated as follows: 94h - len(`XFLE#:`) - 10h = 94h - 6 - 10h = 7Eh.
 *  NOTE: While all path strings *are* NULL-terminated, not every one fills the
 *  remaining space after its name but before its data with all zeros. I'm not
 *  sure what that data is supposed to be, be it permissions, a checksum, or
 *  something else entriely. The old hh extracted these files w/o issue while
 *  ignore this excess data. If it can be determined that the `MAX_PATH_LEN` is
 *  definitely less than 7Eh, this code should be updated so as to store those
 *  extra, unknown bytes in a simple `uint8_t` array of the remaining size.
 *  TODO: Read this value from a config file maybe?
 */
#define MAX_PATH_LEN (0x7E)

struct _IndexFile_s
{
  struct _IdxFileEntry_s
  {
    // Don't really need this
    // uint32_t loc; // Offset in bytes from the start of the file.
    char path[(MAX_PATH_LEN)]; // path to where this file should be extracted.
    /** Offset in bytes from the start of its associated dat file. */
    uint32_t extent_loc;
    uint32_t size; // in bytes
  } *entries;
  size_t size;
  size_t capacity;
};

_IndexFile_t *
_idx_alloc (void)
{
  _IndexFile_t *out = calloc (1, sizeof (_IndexFile_t));
  if (out == NULL)
    return NULL;

  out->capacity = 1;
  out->entries = calloc (out->capacity, sizeof (struct _IdxFileEntry_s));
  if (out->entries == NULL)
    {
      _idx_free (out);
      return NULL;
    }

  return out;
}

void
_idx_free (_IndexFile_t *i)
{
  if (i == NULL)
    return;

  if (i->entries != NULL)
    free (i->entries);
  free (i);
}

static void
_idxe_print (struct _IdxFileEntry_s e[static 1])
{
  fprintf (stdout, "Path: %.*s\n", MAX_PATH_LEN, e->path);
  fprintf (stdout, "Extent location: 0x%08X\n", e->extent_loc);
  fprintf (stdout, "Size: 0x%08X\n", e->size);
}

int
_idx_init (_IndexFile_t *i, const char *path)
{
  if (i == NULL)
    return -1;

  FILE *input_fptr = fopen (path, "rb");
  if (input_fptr == NULL)
    {
      fprintf (stderr, "Error opening index file: %s.\n", path);
      return -1;
    }

  if (fseek (input_fptr, 6, SEEK_CUR) != 0)
    goto fseek_err;

  struct _IdxFileEntry_s entry;
  if ((_hhbr_read_str (input_fptr, entry.path, MAX_PATH_LEN) != 0)
      || (_hhbr_read_le_u32 (input_fptr, &entry.extent_loc) != 0)
      || (_hhbr_read_le_u32 (input_fptr, &entry.size) != 0))
    {
      fclose (input_fptr);
      return -1;
    }

  if (fseek (input_fptr, 4, SEEK_CUR) != 0) // Skip unused zeros.
    goto fseek_err;

  /**
   *  Check repeated size matches original size read. Not sure why this is
   *  duplicated in the first place, but we might as well, right?
   */
  uint32_t value;
  if (_hhbr_read_le_u32 (input_fptr, &value) != 0)
    {
      fclose (input_fptr);
      return -1;
    }

  if (value != entry.size)
    {
      fprintf (stderr,
               "Repeated size value does not match original value read: got "
               "0x%08X, expected 0x%08X\n",
               value, entry.size);
      fclose (input_fptr);
      return -1;
    }

  _idxe_print (&entry);

  fclose (input_fptr);
  return 0;
fseek_err:
  fprintf (stderr, "Error seeking forward in file, %s.\n", path);
  fclose (input_fptr);
  return -1;
}

void
_idx_print (_IndexFile_t *i)
{
  if ((i == NULL) || (i->entries == NULL))
    return;

  for (size_t j = 0; j < i->size; j++)
    {
      fprintf (stdout, "Entry %ld: \n", j);
      _idxe_print (&i->entries[j]);
    }
}
