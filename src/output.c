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
  if (OP_CURRENT_DISK_NAME != NULL)
    {
      if (prepend_path_string (path, OP_CURRENT_DISK_NAME) != 0)
        return HH_MEM_ALLOC_ERROR;
    }

  if (prepend_path_string (path, OP_OUTPUT_DIR) != 0)
    return HH_MEM_ALLOC_ERROR;

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
              printf ("ERROR: failed to create directory, %s\n", path);
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
