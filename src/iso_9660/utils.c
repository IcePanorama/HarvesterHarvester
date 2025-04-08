#include "iso_9660/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
_u_prepend_str (char str[static 1], const size_t str_len,
                const char prefix[static 1], const size_t prefix_len)
{
  char *work = calloc (str_len, sizeof (char));
  if (work == NULL)
    goto out_of_mem_err;

  strncpy (work, str, str_len);
  strncpy (str, prefix, prefix_len);
  strncat (str, work, str_len);

  free (work);
  return 0;
out_of_mem_err:
  fprintf (stderr, "%s: Out of memory error.\n", __func__);
  return -1;
}
