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
#include "output.h"
#include "errors.h"
#include "options.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#endif

int8_t
create_output_directory (char *path)
{
  if (CURRENT_DISK_NAME != NULL)
    {
      if (prepend_path_string (path, CURRENT_DISK_NAME) != 0)
        return HH_MEM_ALLOC_ERROR;
    }

  if (prepend_path_string (path, OP_OUTPUT_DIR) != 0)
    return HH_MEM_ALLOC_ERROR;

  create_new_output_directory (path);

  return 0;
}

int8_t
create_new_output_directory (const char *path)
{
  if (directory_exists (path))
    return 0;

  // Why +2 here? '\0' and ?
  char *tmp = calloc (strlen (path) + 2, sizeof (char));
  if (tmp == NULL)
    {
      fprintf (stderr, CALLOC_FAILED_ERR_MSG_FMT, strlen (path) + 2);
      return HH_MEM_ALLOC_ERROR;
    }

  strcpy (tmp, path);
  char *token = strtok (tmp, &OP_PATH_SEPARATOR);

  char *dir = calloc (strlen (path) + 2, sizeof (char));
  if (dir == NULL)
    {
      fprintf (stderr, CALLOC_FAILED_ERR_MSG_FMT, strlen (path) + 2);
      free (tmp);
      return HH_MEM_ALLOC_ERROR;
    }

  while (token != NULL)
    {
      strcat (dir, token);

      if (!directory_exists (dir))
        {
          // TODO: test on Windows
          int status;
#ifdef _WIN32
          status = _mkdir (dir);
#else
          status = mkdir (dir, 0700);
#endif
          if (status != 0)
            {
              fprintf (stderr,
                       "[HarvesterHarvester]ERROR: failed to create the "
                       "following output "
                       "directory, %s\n.",
                       path);
              free (dir);
              free (tmp);
              return HH_CREATE_OUTPUT_DIR_ERROR;
            }
        }

      strcat (dir, &OP_PATH_SEPARATOR);
      token = strtok (NULL, &OP_PATH_SEPARATOR);
    }

  free (dir);
  free (tmp);
  return 0;
}
