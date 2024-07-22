#include "utils.h"
#include "options.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
prepend_path_string (char *str, const char *prefix)
{
  if (strcmp (prefix, (const char *)"") == 0
      || strcmp (prefix, (const char *)"\1") == 0)
    {
      return;
    }

  char *tmp = calloc (strlen (str) + strlen (prefix) + 2, sizeof (char));
  if (tmp == NULL)
    {
      perror ("ERROR: unable to calloc tmp string");
      exit (1);
    }
  strcpy (tmp, str);
  strcpy (str, prefix);
  strcat (str, &OP_PATH_SEPARATOR);
  strcat (str, tmp);
  free (tmp);
}

bool
is_string_dat_file (char *str)
{
  const char *EXPECTED_EXTENSION = ".DAT";
  const uint8_t EXTENSION_LEN = 5; // +1 for null terminator

  size_t len = strlen (str) + 1;
  return (len > EXTENSION_LEN
          && strcmp (str + (len - EXTENSION_LEN), EXPECTED_EXTENSION) == 0);
}
