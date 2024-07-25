#include "utils.h"
#include "data_reader.h"
#include "options.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

int8_t
prepend_path_string (char *str, const char *prefix)
{
  if (strcmp (prefix, (const char *)"") == 0
      || strcmp (prefix, (const char *)"\1") == 0)
    {
      return 0;
    }

  char *tmp = calloc (strlen (str) + strlen (prefix) + 2, sizeof (char));
  if (tmp == NULL)
    {
      perror ("ERROR: unable to calloc tmp string");
      return -1;
    }

  strcpy (tmp, str);
  strcpy (str, prefix);
  strcat (str, &OP_PATH_SEPARATOR);
  strcat (str, tmp);
  free (tmp);

  return 0;
}

void
prepend_string (char *str, const char *prefix)
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

bool
file_exists (const char *filename)
{
#ifdef _WIN32
  DWORD attribs = GetFileAttributes (filename);
  return attribs != INVALID_FILE_ATTRIBUTES
         && !(attribs & FILE_ATTRIBUTE_DIRECTORY);
#else
  return access (filename, F_OK) != -1;
#endif
}

/** On error, returns false. */
bool
peek_char_is (FILE *fptr, uint8_t expected_ch)
{
  uint8_t byte;
  if (read_single_uint8 (fptr, &byte) != 0)
    {
      return false;
    }
  else if (byte == expected_ch)
    {
      return true;
    }

  fseek (fptr, -1, SEEK_CUR);
  return false;
}

/** This is sort of a hacky solution. TODO: implement better err handling. */
bool
peek_eof (FILE *fptr)
{
  uint8_t value;
  size_t bytes_read = fread (&value, sizeof (uint8_t), 1, fptr);
  fseek (fptr, -1, SEEK_CUR);
  return bytes_read == 0;
}

uint16_t
change_endianness_uint16 (uint16_t value)
{
  return (value << 8) | (value >> 8);
}
