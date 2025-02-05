#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int_fast8_t
u_prepend_path_str (char **str, const char prefix[static 1], size_t prefix_len)
{
  if (str == NULL || (*str) == NULL)
    {
      fprintf (stderr, "ERROR: Given string is null!");
      return -1;
    }
  // ignore empty prefixes (e.g., root directories)
  else if (prefix[0] == '\0')
    return 0;

  // + 2 for path separator & null terminator.
  size_t new_str_len = strlen ((*str)) + prefix_len + 2;
  char *output = calloc (new_str_len, sizeof (char));
  if (output == NULL)
    {
      fprintf (stderr, "ERROR: Unable to alloc string of size, %ld.\n",
               new_str_len);
      return -1;
    }

  strncpy (output, prefix, prefix_len);
#ifndef _WIN32
  strcat (output, "/");
#else  /* _WIN32 */
  strcat (output, "\\");
#endif /* _WIN32 */
  strcat (output, (*str));

  char *tmp = realloc ((*str), new_str_len);
  if (tmp == NULL)
    {
      fprintf (stderr, "ERROR: Failed to resize given string to size, %ld.\n",
               new_str_len);
      free (output);
      return -1;
    }
  (*str) = tmp;

  strcpy ((*str), output);
  free (output);
  return 0;
}
