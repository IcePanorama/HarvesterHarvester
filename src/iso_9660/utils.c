#include "iso_9660/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int
_u_prepend_str (char str[static 1], const size_t str_len,
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

int
_u_create_export_dir (const char path[static 1])
{
  char *path_cpy = malloc (sizeof (char) * strlen (path) + 1);
  if (path_cpy == NULL)
    {
      fprintf (stderr, "%s: out of memory error.\n", __func__);
      return -1;
    }

  char *curr_path = calloc (sizeof (char) * strlen (path) + 1, sizeof (char));
  if (curr_path == NULL)
    {
      fprintf (stderr, "%s: out of memory error.\n", __func__);
      free (path_cpy);
      return -1;
    }

  strcpy (path_cpy, path);
  char *tok = strtok (path_cpy, "/");
  while (tok != NULL)
    {
      if (strchr (tok, '.') != NULL) // skip files.
        break;

      strcat (curr_path, tok);
      struct stat path_info;
      if (stat (curr_path, &path_info) != 0)
        {
          int status = mkdir (curr_path, 0700);
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
_u_export_data (uint8_t data[static 1], size_t data_size,
                const char path[static 1])
{
  // FIXME: Can `_u_create_export_dir` be a static function?
  if (_u_create_export_dir (path) != 0)
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
