#include "output.h"
#include "path_table.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

const char *OUTPUT_DIR = "output";

/*
 *  create_output_directories
 *
 *  TODO: add documentation
 */
void
create_output_directory (char *path)
{
  prepend_path_string (path, OUTPUT_DIR);

  char *tmp = calloc (strlen (path) + 2, sizeof (char));
  if (tmp == NULL)
    {
      perror ("ERROR: unable to calloc tmp string");
      exit (1);
    }
  strcpy (tmp, path);

  char *token = strtok (tmp, "/");

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

      strcat (dir, "/");
      token = strtok (NULL, "/");
    }

  free (dir);
  free (tmp);
}

/*
 *  directory_exists
 *
 *  TODO: add documentation.
 */
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
