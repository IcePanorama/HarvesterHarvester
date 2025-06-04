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
#include "harvester_harvester/hh.h"
#include "harvester_harvester/idx_file.h"
#include "harvester_harvester/known_files.h"
#include "iso_9660.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const HHOptions_t HH_DEFAULT_OPTIONS
    = { .processing_mode = _HHPM_NORMAL_PROCESSING };

/**
 *  Constructs a string (`full_path`) that is `output_path` + '/' +
 *  `file_path`. Assumes `full_path` is not NULL and is NULL-terminated.
 *
 *  Param:  full_path  stores the output of this function
 */
static void
build_path_str (char *full_path, const char *output_path,
                const char *file_path)
{
  strcpy (full_path, output_path);
  strcat (full_path, "/");
  strcat (full_path, file_path);
}

/**
 *  Extracts an internal dat file (`dat_path`) to `output_path` using the index
 *  file at `idx_path`.
 *
 *  Return:  Zero on success, non-zero on failure.
 */
static int
extract_int_dat (const char *output_path, const char *dat_path,
                 const char *idx_path)
{
  _HHIndexFile_t *idx = _hhidx_alloc ();
  if (idx == NULL)
    goto oom_error;

  size_t output_len = strlen (output_path);

  // +2 for path separator and NULL terminator.
  size_t full_idx_path_len = (output_len + strlen (idx_path)) + 2;
  char *full_idx_path = calloc (full_idx_path_len, sizeof (char));
  if (full_idx_path == NULL)
    goto oom_error;

  printf ("Processing internal index file: %s\n", idx_path);
  build_path_str (full_idx_path, output_path, idx_path);
  if (_hhidx_init (idx, full_idx_path) != 0)
    {
      _hhidx_free (idx);
      free (full_idx_path);
      return -1;
    }
  free (full_idx_path);

  // +2 for path separator and NULL terminator.
  size_t full_dat_path_len = (output_len + strlen (dat_path)) + 2;
  char *full_dat_path = calloc (full_dat_path_len, sizeof (char));
  if (full_dat_path == NULL)
    goto oom_error;

  printf ("Extracting internal dat file: %s\n", dat_path);
  build_path_str (full_dat_path, output_path, dat_path);
  if (_hhidx_extract (idx, full_dat_path) != 0)
    {
      _hhidx_free (idx);
      free (full_dat_path);
      return -1;
    }

  free (full_dat_path);
  _hhidx_free (idx);
  return 0;
oom_error:
  fprintf (stderr, "%s: Out of memory error.\n", __func__);
  return -1;
}

/**
 *  Retrieves the internal dat file path at the end of `input_path`, which
 *  should begin with `DISK#/`, where '#' is some number. If `DISK#/` is not
 *  found, it returns a pointer to the NULL-terminator of `input_path`.
 *
 *  Return:  a "canonical" internal dat file path
 */
static char *
get_canonical_input_path (const char *input_path)
{
  const char *key = "DISK#/";
  char *work = NULL;
  for (work = (char *)input_path; (work != NULL) && (*work != '\0'); work++)
    {
      if (*work == *key)
        {
          char *key_ptr = (char *)(key) + 1;
          char *tmp = (char *)(work) + 1;
          while ((key_ptr != NULL) && (tmp != NULL)
                 && ((*key_ptr == *tmp) || (*key_ptr == '#')))
            {
              key_ptr++;
              tmp++;
            }

          if ((key_ptr != NULL) && (*key_ptr == '\0') && (tmp != NULL))
            break;
        }
    }

  return work;
}

/**
 *  When HH is given a path to some internal dat file as input, this func does
 *  initialize everything needed for calling `extract_int_dat`.
 *
 *  Return:  Zero on success, non-zero on failure.
 */
static int
extraction_w_int_dat_input (const char *input_path, const char *output_path)
{
  char *work = get_canonical_input_path (input_path);
  if ((work == NULL) || (!_hhkf_is_known_int_dat (work)))
    {
      fprintf (stderr, "Unrecognized internal dat file: %s. Aborting.\n",
               input_path);
      return -1;
    }

  const char *assoc_idx_path = _hhkf_get_idx_path_from_int_dat (work);
  if ((assoc_idx_path == NULL)
      || (extract_int_dat (output_path, work, assoc_idx_path) != 0))
    return -1;

  return 0;
}

/**
 *  Extracts an I9660 file system (`input_path`) to `output_path`.
 *
 *  Returns: Zero on success, non-zero on failure.
 */
static int
extract_i9660_fs (const char *input_path, const char *output_path)
{
  ISO9660FileSystem_t *fs = i9660_alloc ();
  if (fs == NULL)
    {
      fprintf (stderr, "Out of memory error.\n");
      return -1;
    }

  FILE *input_file = fopen (input_path, "rb");
  if (input_file == NULL)
    {
      fprintf (stderr, "Error opening file %s.\n", input_path);
      i9660_free (fs);
      return -1;
    }

  if ((i9660_init (fs, input_file) != 0)
      || (i9660_extract (fs, input_file, output_path) != 0))
    {
      fclose (input_file);
      i9660_free (fs);
      return -1;
    }

  fclose (input_file);
  i9660_free (fs);
  return 0;
}

/**
 *  Extracts a known I9660 file system, as well as its internal dat files,
 *  `opts` permitting. This function differs from `extract_i9660_fs` as it
 *  attempts to handle those internal dat file automatically.
 */
static int
known_i9660_extraction (const char *input_path, const char *output_path,
                        const HHOptions_t *opts)
{
  if (extract_i9660_fs (input_path, output_path) != 0)
    return -1;

  if (opts->processing_mode == _HHPM_SKIP_INTERNAL_DATS)
    return 0;

  const char **int_dat_files = _hhkf_get_int_dat_paths_from_i9660 (input_path);
  const char **idx_files = _hhkf_get_int_idx_paths_from_i9660 (input_path);
  if ((int_dat_files == NULL) || (idx_files == NULL))
    return -1;

  for (size_t i = 0; i < 3; i++)
    {
      if (extract_int_dat (output_path, int_dat_files[i], idx_files[i]) != 0)
        return -1;
    }

  return 0;
}

int
hh_extract_filesystem_w_options (const char input_path[static 1],
                                 const char output_path[static 1],
                                 const HHOptions_t opts[static 1])
{
  if ((opts->processing_mode != _HHPM_SKIP_I9660_DATS)
      && (_hhkf_is_known_i9660_file (input_path)))
    {
      return known_i9660_extraction (input_path, output_path, opts);
    }
  else if (opts->processing_mode == _HHPM_SKIP_I9660_DATS)
    {
      return extraction_w_int_dat_input (input_path, output_path);
    }
  else
    {
      fprintf (stderr,
               "Unrecognized file: %s. Attempting to extract as ISO 9660 file "
               "system.\n",
               input_path);

      if (extract_i9660_fs (input_path, output_path) != 0)
        return -1;
    }

  return 0;
}

int
hh_extract_filesystem (const char input_path[static 1],
                       const char output_path[static 1])
{
  return hh_extract_filesystem_w_options (input_path, output_path,
                                          &HH_DEFAULT_OPTIONS);
}
