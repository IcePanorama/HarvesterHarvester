#include "harvester_harvester/hh.h"
#include "harvester_harvester/idx_file.h"
#include "harvester_harvester/known_files.h"
#include "harvester_harvester/options.h"
#include "iso_9660.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const HHOptions_t HH_DEFAULT_OPTIONS
    = { .processing_mode = _HHPM_NORMAL_PROCESSING };

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
      i9660_free (fs);
      fclose (input_file);
      return -1;
    }

  i9660_free (fs);
  fclose (input_file);
  return 0;
}

static int
extract_int_dat (const char *output_path, const char *dat_path,
                 const char *idx_path)
{
  _IndexFile_t *idx = _idx_alloc ();
  if (idx == NULL)
    goto oom_error;

  size_t output_len = strlen (output_path);
  // +2 for path separator and NULL terminator.
  char *full_idx_path
      = calloc ((output_len + strlen (idx_path)) + 2, sizeof (char));
  if (full_idx_path == NULL)
    goto oom_error;
  strcpy (full_idx_path, output_path);
  strcat (full_idx_path, "/");
  strcat (full_idx_path, idx_path);

  printf ("Extracting internal dat file: %s\n", dat_path);
  if (_idx_init (idx, full_idx_path) != 0)
    {
      _idx_free (idx);
      free (full_idx_path);
      return -1;
    }
  free (full_idx_path);

  // +2 for path separator and NULL terminator.
  char *full_dat_path
      = calloc ((output_len + strlen (dat_path)) + 2, sizeof (char));
  if (full_dat_path == NULL)
    goto oom_error;
  strcpy (full_dat_path, output_path);
  strcat (full_dat_path, "/");
  strcat (full_dat_path, dat_path);

  if (_idx_extract (idx, full_dat_path) != 0)
    {
      _idx_free (idx);
      free (full_dat_path);
      return -1;
    }
  free (full_dat_path);

  _idx_free (idx);
  return 0;
oom_error:
  fprintf (stderr, "%s: Out of memory error.\n", __func__);
  return -1;
}

int
hh_extract_filesystem (const char input_path[static 1],
                       const char output_path[static 1])
{
  return hh_extract_filesystem_w_options (input_path, output_path,
                                          HH_DEFAULT_OPTIONS);
}

static int
known_i9660_extraction (const char input_path[static 1],
                        const char output_path[static 1],
                        const HHOptions_t opts)
{
  if (extract_i9660_fs (input_path, output_path) != 0)
    return -1;

  if (opts.processing_mode == _HHPM_SKIP_INTERNAL_DATS)
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

/**
 *  Retrieves the internal dat file path at the end of `input_path`, which
 *  should begin with `DISK#/`. If `DISK#/` is not found, it returns a pointer
 *  to the NULL-terminator of `input_path`.
 */
static char *
get_input_path_end (const char *input_path)
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
 *  Presuming `input_path` points to some known, internal dat file in this
 *  case.
 */
static int
extraction_w_int_dat_input (const char input_path[static 1],
                            const char output_path[static 1])
{
  char *work = get_input_path_end (input_path);
  if (work == NULL)
    {
      fprintf (stderr, "Unrecognized internal dat file: %s. Aborting.\n",
               input_path);
      return -1;
    }
  else if (!_hhkf_is_known_int_dat (work))
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

int
hh_extract_filesystem_w_options (const char input_path[static 1],
                                 const char output_path[static 1],
                                 const HHOptions_t opts)
{
  if ((opts.processing_mode != _HHPM_SKIP_I9660_DATS)
      && (_hhkf_is_known_i9660_file (input_path)))
    {
      return known_i9660_extraction (input_path, output_path, opts);
    }
  else if (opts.processing_mode == _HHPM_SKIP_I9660_DATS)
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
