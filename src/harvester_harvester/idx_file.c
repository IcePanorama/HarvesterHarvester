/**
 *  Copyright (C) 2024-2025  IcePanorama
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "harvester_harvester/idx_file.h"
#include "iso_9660/binary_reader.h"
#include "iso_9660/utils.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 *  Each entry is 94h bytes. Each entry contains `XFLE#:`, followed by a NULL-
 *  terminated path string, and finally 10h bytes of data. `MAX_PATH_LEN` is
 *  calculated as follows: 94h - len(`XFLE#:`) - 10h = 94h - 6 - 10h = 7Eh.
 *
 *  NOTE: While all path strings *are* NULL-terminated, not every one fills the
 *  remaining space after its name but before its data with all zeros. I'm not
 *  sure what that data is supposed to be, be it permissions, a checksum, or
 *  something else entriely. The old hh extracted these files w/o issue while
 *  ignoring this excess data though.
 *
 *  See: `harvester_harvester/idx_file.h`
 *  See: `.github/extra_idx_data.md`
 */
#define MAX_PATH_LEN (0x7E)

struct _HHIndexFile_s
{
  char *file_path;  // Path to idx file itself.
  char *output_dir; // Path to idx file's dir. E.g., `output/DISK1/`.

  struct _IdxFileEntry_s
  {
    char path[(MAX_PATH_LEN)]; // path to where this file should be extracted.
    /** Offset in bytes from the start of its associated dat file. */
    uint32_t extent_loc;
    uint32_t size; // in bytes
  } *entries;
  size_t size;     // logical size of entries
  size_t capacity; // physical size of entries
};

_HHIndexFile_t *
_hhidx_alloc (void)
{
  _HHIndexFile_t *out = calloc (1, sizeof (_HHIndexFile_t));
  if (out == NULL)
    return NULL;

  out->capacity = 1;
  out->entries = calloc (out->capacity, sizeof (struct _IdxFileEntry_s));
  if (out->entries == NULL)
    {
      _hhidx_free (out);
      return NULL;
    }

  return out;
}

void
_hhidx_free (_HHIndexFile_t *i)
{
  if (i == NULL)
    return;

  if (i->file_path != NULL)
    free (i->file_path);
  if (i->output_dir != NULL)
    free (i->output_dir);
  if (i->entries != NULL)
    free (i->entries);
  free (i);
}

/**
 *  NOTE: using `fprintf` here to make switching to file logging easier in the
 *  future (inb4 YAGNI, probably).
 */
static void
_hhidxe_print (struct _IdxFileEntry_s e[static 1])
{
  fprintf (stdout, "Path: %.*s\n", MAX_PATH_LEN, e->path);
  fprintf (stdout, "Extent location: 0x%08X\n", e->extent_loc);
  fprintf (stdout, "Size: 0x%08X\n", e->size);
}

void
_hhidx_print (_HHIndexFile_t *i)
{
  if ((i == NULL) || (i->entries == NULL))
    return;

  fprintf (stdout, "%s: \n", i->file_path);
  for (size_t j = 0; j < i->size; j++)
    {
      fprintf (stdout, "Entry %zu: \n", j);
      _hhidxe_print (&i->entries[j]);
    }
}

/**
 *  Appends an index file entry (`e`) to the entries list of `i`.
 *
 *  Returns: Zero on success, non-zero on failure.
 */
static int
append_entry (_HHIndexFile_t *i, const struct _IdxFileEntry_s *e)
{
  if (i->size == i->capacity)
    {
      size_t new_cap = i->capacity * 2;
      struct _IdxFileEntry_s *tmp
          = realloc (i->entries, new_cap * sizeof (struct _IdxFileEntry_s));
      if (tmp == NULL)
        return -1;

      i->capacity = new_cap;
      i->entries = tmp;
    }

  memcpy (&i->entries[i->size], e, sizeof (struct _IdxFileEntry_s));
  i->size++;

  return 0;
}

/**
 *  Checks whether `fptr` is at the end of its file.
 *
 *  Returns:  True if yes, else false
 */
static bool
peek_eof (FILE *fptr)
{
  int b = fgetc (fptr);
  if (b == EOF)
    return true;

  ungetc (b, fptr);

  return false;
}

/** Change all occurences of '\\' to '/' in a given string. */
static void
flip_path_separators (char *str)
{
  char *work = str;
  while (*work != '\0')
    {
      if (*work == '\\')
        *work = '/';
      work++;
    }
}

/**
 *  Initializes a single index file entry (`e`), using data from `input_fptr`.
 *
 *  Returns:  Zero on success, non-zero on failure.
 */
static int
init_entry (struct _IdxFileEntry_s *e, FILE *input_fptr)
{
  if (fseek (input_fptr, 6, SEEK_CUR) != 0) // Seek past `XFILE#:`
    goto fseek_err;

  if (_i9660br_read_str (input_fptr, e->path, MAX_PATH_LEN) != 0)
    return -1;

  flip_path_separators (e->path);

  if ((_i9660br_read_le_u32 (input_fptr, &e->extent_loc) != 0)
      || (_i9660br_read_le_u32 (input_fptr, &e->size) != 0))
    {
      return -1;
    }

  if (fseek (input_fptr, 4, SEEK_CUR) != 0) // Skip unused zeros.
    goto fseek_err;

  /**
   *  Check repeated size matches original size read. Not sure why this is
   *  duplicated in the first place, but we might as well, right?
   */
  uint32_t value;
  if (_i9660br_read_le_u32 (input_fptr, &value) != 0)
    {
      return -1;
    }
  else if (value != e->size)
    {
      fprintf (stderr,
               "Repeated size value does not match original value read: got "
               "0x%08X, expected 0x%08X\n",
               value, e->size);
      return -1;
    }

  return 0;
fseek_err:
  fprintf (stderr, "Error seeking forward in index file.\n");
  return -1;
}

/**
 *  Finds the path of the directory in which `i` exists.
 *
 *  Returns:  Zero on success, non-zero on failure.
 */
/*
static int
find_dir_path (_HHIndexFile_t *i, const char *path)
{
  char *end = strrchr (path, '/');
  if (end == NULL)
    {
      fprintf (stderr,
               "Given path string, %s, does not contain path separator, %c.\n",
               path, '/');
      return -1;
    }

  size_t new_len = (size_t)(end - path);
  i->output_dir
      = calloc (new_len + 1, sizeof (char)); // +1 for NULL-terminator
  if (i->output_dir == NULL)
    {
      fprintf (stderr, "%s: out of memory error.\n", __func__);
      return -1;
    }
  strncpy (i->output_dir, path, new_len);

  return 0;
}
*/

int
_hhidx_init (_HHIndexFile_t *i, const char path[static 1],
             const char output_dir[static 1])
{
  if (i == NULL) // || (find_dir_path (i, path) != 0))
    return -1;

  const size_t dir_path_len = strlen (path) + 1; // +1 for NULL-terminator
  i->output_dir = calloc (dir_path_len, sizeof (char));
  if (i->output_dir == NULL)
    {
      fprintf (stderr, "%s: out of memory error.\n", __func__);
      return -1;
    }
  strcpy (i->output_dir, output_dir);

  const size_t file_path_len = strlen (path) + 1; // +1 for NULL-terminator
  i->file_path = calloc (file_path_len, sizeof (char));
  if (i->file_path == NULL)
    {
      fprintf (stderr, "%s: out of memory error.\n", __func__);
      return -1;
    }
  strcpy (i->file_path, path);

  FILE *input_fptr = fopen (path, "rb");
  if (input_fptr == NULL)
    {
      fprintf (stderr, "Error opening index file: %s.\n", path);
      return -1;
    }

  while (!peek_eof (input_fptr))
    {
      struct _IdxFileEntry_s entry;
      if (init_entry (&entry, input_fptr) != 0)
        goto loop_err_exit;

      if (append_entry (i, &entry) != 0)
        goto loop_err_exit;
    }

  fclose (input_fptr);
  return 0;
loop_err_exit:
  fclose (input_fptr);
  return -1;
}

/*
int
_hhidx_init_w_dir_path (_HHIndexFile_t *i, const char path[static 1], const
char output_dir[static 1])
{
  if (i == NULL)
    return -1;

  const size_t dir_path_len = strlen (path) + 1; // +1 for NULL-terminator
  i->output_dir = calloc (dir_path_len, sizeof (char));
  if (i->output_dir == NULL)
    {
      fprintf (stderr, "%s: out of memory error.\n", __func__);
      return -1;
    }
  strcpy (i->output_dir, output_dir);

  return 0;
}
*/

/**
 *  Extracts a single index entry (`e`) to `output_dir` using data from
 *  `dat_fptr`.
 *
 *  Returns: Zero on success, non-zero on failure.
 */
static int
extract_idx_entry (struct _IdxFileEntry_s *e, FILE *dat_fptr,
                   const char *output_dir)
{
  if (fseek (dat_fptr, e->extent_loc, SEEK_SET) != 0)
    {
      fprintf (
          stderr,
          "fseek error: Failed to seek to extent location of %s (%08X).\n",
          e->path, e->extent_loc);
      return -1;
    }

  uint8_t *data = malloc (e->size);
  if (data == NULL)
    goto oom_error;

  if (_i9660br_read_u8_array (dat_fptr, data, e->size) != 0)
    {
      fprintf (stderr, "Error reading data for %s.\n", e->path);
      return -1;
    }

  char *path
      = calloc (strlen (output_dir) + strlen (e->path) + 1, sizeof (char));
  if (path == NULL)
    goto oom_error;
  strcpy (path, output_dir);
  strcat (path, e->path);

  printf ("Extracting file: %s\n", path);
  if (_i9660u_export_data (data, e->size, path) != 0)
    {
      free (path);
      free (data);
      return 1;
    }

  free (path);
  free (data);
  return 0;
oom_error:
  fprintf (stderr, "%s: Out of memory error.\n", __func__);
  return -1;
}

int
_hhidx_extract (_HHIndexFile_t *i, const char *dat_path)
{
  if (i == NULL)
    return -1;

  FILE *dat_fptr = fopen (dat_path, "rb");
  if (dat_fptr == NULL)
    {
      fprintf (stderr, "Dat file does not exist: %s.\n", dat_path);
      return -1;
    }

  printf ("Extracting index file: %s\n", i->file_path);
  for (size_t j = 0; j < i->size; j++)
    {
      if (extract_idx_entry (&i->entries[j], dat_fptr, i->output_dir) != 0)
        {
          fprintf (stderr, "Error extracting index file entry: %s%s.\n",
                   i->output_dir, i->entries[j].path);
          fclose (dat_fptr);
          return -1;
        }
    }

  fclose (dat_fptr);
  return 0;
}
