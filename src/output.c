#include "output.h"
#include "options.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#endif

// const char *OP_OUTPUT_DIR = "output";

void
create_output_directory (char *path)
{
  if (OP_CURRENT_DISK_NAME != NULL)
    prepend_path_string (path, OP_CURRENT_DISK_NAME);

  prepend_path_string (path, OP_OUTPUT_DIR);

  char *tmp = calloc (strlen (path) + 2, sizeof (char));
  if (tmp == NULL)
    {
      perror ("ERROR: unable to calloc tmp string");
      exit (1);
    }
  strcpy (tmp, path);

  char *token = strtok (tmp, &OP_PATH_SEPARATOR);

  char *dir = calloc (strlen (path) + 2, sizeof (char));
  if (dir == NULL)
    {
      perror ("ERROR: unable to calloc dir string");
      exit (1);
    }

  while (token != NULL)
    {
      strcat (dir, token);

      if (!directory_exists (dir))
        {
          // TODO: test me on Windows
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
              exit (1);
            }
        }

      strcat (dir, &OP_PATH_SEPARATOR);
      token = strtok (NULL, &OP_PATH_SEPARATOR);
    }

  free (dir);
  free (tmp);
}

bool
directory_exists (const char *dir)
{
#ifdef _WIN32
  struct _stat st;
  return (_stat (dir, &st) == 0 && (st.st_mode & _S_IFDIR));
#else
  struct stat st;
  return (stat (dir, &st) == 0 && S_ISDIR (st.st_mode));
#endif
}
