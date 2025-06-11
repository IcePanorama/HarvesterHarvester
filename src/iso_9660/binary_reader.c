/**
 *  Copyright (C) 2024-2025  IcePanorama
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "iso_9660/binary_reader.h"

int
_i9660br_read_u8 (FILE *fptr, uint8_t *output)
{
  size_t bytes_read = fread (output, sizeof (uint8_t), 1, fptr);
  if (bytes_read != sizeof (uint8_t))
    {
      fprintf (stderr, "Error reading uint8 from file.\n");
      return -1;
    }

  return 0;
}

int
_i9660br_read_le_u16 (FILE *fptr, uint16_t *output)
{
  uint8_t bytes[2] = { 0 };
  for (size_t i = 0; i < 2; i++)
    {
      if (_i9660br_read_u8 (fptr, &bytes[i]) != 0)
        return -1;
    }

  *output = bytes[0];
  *output |= (uint16_t)(bytes[1] << 8);

  return 0;
}

int
_i9660br_read_be_u16 (FILE *fptr, uint16_t *output)
{
  uint8_t bytes[2] = { 0 };
  for (size_t i = 0; i < 2; i++)
    {
      if (_i9660br_read_u8 (fptr, &bytes[i]) != 0)
        return -1;
    }

  *output = bytes[1];
  *output |= (uint16_t)(bytes[0] << 8);

  return 0;
}

int
_i9660br_read_le_be_u16 (FILE *fptr, uint16_t *output)
{
  if (_i9660br_read_le_u16 (fptr, output) != 0)
    return -1;

  uint16_t value = 0;
  if (_i9660br_read_be_u16 (fptr, &value) != 0)
    return -1;
  else if (*output != value)
    {
      fprintf (stderr,
               "Little endian 16-bit value read from file does not match the "
               "big endian value which follows it. Got %04X, expected %04X.\n",
               value, *output);
      return -1;
    }

  return 0;
}

int
_i9660br_read_le_u32 (FILE *fptr, uint32_t *output)
{
  uint8_t bytes[4] = { 0 };
  for (size_t i = 0; i < 4; i++)
    {
      if (_i9660br_read_u8 (fptr, &bytes[i]) != 0)
        return -1;
    }

  *output = bytes[0];
  *output |= (uint32_t)(bytes[1] << 8);
  *output |= (uint32_t)(bytes[2] << 16);
  *output |= (uint32_t)(bytes[3] << 24);

  return 0;
}

int
_i9660br_read_be_u32 (FILE *fptr, uint32_t *output)
{
  uint8_t bytes[4] = { 0 };
  for (size_t i = 0; i < 4; i++)
    {
      if (_i9660br_read_u8 (fptr, &bytes[i]) != 0)
        return -1;
    }

  *output = bytes[3];
  *output |= (uint32_t)(bytes[2] << 8);
  *output |= (uint32_t)(bytes[1] << 16);
  *output |= (uint32_t)(bytes[0] << 24);

  return 0;
}

int
_i9660br_read_le_be_u32 (FILE *fptr, uint32_t *output)
{
  if (_i9660br_read_le_u32 (fptr, output) != 0)
    return -1;

  uint32_t value = 0;
  if (_i9660br_read_be_u32 (fptr, &value) != 0)
    return -1;
  else if (*output != value)
    {
      fprintf (stderr,
               "Little endian 32-bit value read from file does not match the "
               "big endian value which follows it. Got %08X, expected %08X.\n",
               value, *output);
      return -1;
    }

  return 0;
}

int
_i9660br_read_str (FILE *fptr, char *output, size_t length)
{
  size_t bytes_read = fread (output, sizeof (char), length, fptr);
  if (bytes_read != (sizeof (char) * length))
    {
      fprintf (stderr, "Error reading string from file.\n");
      return -1;
    }

  return 0;
}

int
_i9660br_read_u8_array (FILE *fptr, uint8_t *output, size_t length)
{
  size_t bytes_read = fread (output, sizeof (uint8_t), length, fptr);
  if (bytes_read != sizeof (uint8_t) * length)
    {
      fprintf (stderr, "Error reading uint8 array of size %zu from file.\n",
               length);
      return -1;
    }

  return 0;
}
