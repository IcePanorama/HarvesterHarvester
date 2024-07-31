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
#include "utils.h"
#include "data_reader.h"
#include "errors.h"
#include "options.h"

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
      return HH_MEM_ALLOC_ERROR;
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
