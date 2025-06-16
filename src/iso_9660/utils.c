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
#include "iso_9660/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef __linux__
#define MKDIR(path) (mkdir (path, 0700))
#define STAT(path, buff) (stat (path, buff))

typedef struct stat stat_t;
#else /* not __linux__ */
#include <direct.h>
#include <sys/types.h>

#define MKDIR(path) (_mkdir (path))
#define STAT(path, buff) (_stat (path, buff))

typedef struct _stat stat_t;
#endif /* not __linux__ */

int
_i9660u_prepend_str (char str[static 1], const size_t str_len,
                     const char prefix[static 1], const size_t prefix_len)
{
  char *work = calloc (str_len, sizeof (char));
  if (work == NULL)
    goto out_of_mem_err;

  strncpy (work, str, str_len);
  memset (str, 0, sizeof (char) * str_len);
  strncpy (str, prefix, prefix_len);
  strncat (str, work, str_len);

  free (work);
  return 0;
out_of_mem_err:
  fprintf (stderr, "%s: Out of memory error.\n", __func__);
  return -1;
}

/**
 *  Recursively creates directories along `path`.
 *  Param:  path  a NULL-terminated path string for a directory
 *  Returns: Zero on success, non-zero on failure.
 */
static int
create_export_dir (const char path[static 1])
{
  char *path_cpy = malloc (sizeof (char) * strlen (path) + 1);
  if (path_cpy == NULL)
    {
      fprintf (stderr, "%s: out of memory error.\n", __func__);
      return -1;
    }
  strcpy (path_cpy, path);

  char *curr_path = calloc (strlen (path) + 1, sizeof (char));
  if (curr_path == NULL)
    {
      fprintf (stderr, "%s: out of memory error.\n", __func__);
      free (path_cpy);
      return -1;
    }

  char *tok = strtok (path_cpy, "/");
  while (tok != NULL)
    {
      if (strchr (tok, '.') != NULL) // skip files.
        break;

      strcat (curr_path, tok);
      stat_t path_info;
      if ((STAT (curr_path, &path_info)) != 0)
        {
          int status = (MKDIR (curr_path));
          if (status != 0)
            {
              fprintf (stderr, "Failed to create output directory: %s.\n",
                       curr_path);
              free (curr_path);
              free (path_cpy);
              return -1;
            }
        }

      tok = strtok (NULL, "/");
      if (tok != NULL)
        strcat (curr_path, "/");
    }

  free (curr_path);
  free (path_cpy);
  return 0;
}

int
_i9660u_export_data (uint8_t data[static 1], const size_t data_size,
                     const char path[static 1])
{
  if (create_export_dir (path) != 0)
    return -1;

  FILE *output_file = fopen (path, "wb");
  if (output_file == NULL)
    {
      fprintf (stderr, "Failed to open file for export: %s.\n", path);
      return -1;
    }

  int status = 0;
  if (fwrite (data, sizeof (uint8_t), data_size, output_file) != data_size)
    {
      fprintf (stderr, "Error exporting file, %s.\n", path);
      status = -1; // still need to attempt `fclose` below.
    }

  if (fclose (output_file) != 0)
    {
      fprintf (stderr, "Error closing file, %s.\n", path);
      return -1;
    }

  return status;
}
