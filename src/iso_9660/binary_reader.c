#include "iso_9660/binary_reader.h"
#include <stdio.h>

int
_br_read_u8 (FILE *fptr, uint8_t *output)
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
_br_read_str (FILE *fptr, char *output, size_t length)
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
_br_read_le_be_u32 (FILE *fptr, uint32_t *output)
{
  if (_br_read_le_u32 (fptr, output) != 0)
    return -1;

  uint32_t value = 0;
  if (_br_read_be_u32 (fptr, &value) != 0)
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
_br_read_le_u32 (FILE *fptr, uint32_t *output)
{
  uint8_t bytes[4] = { 0 };
  for (size_t i = 0; i < 4; i++)
    {
      if (_br_read_u8 (fptr, &bytes[i]) != 0)
        return -1;
    }

  *output = bytes[0];
  *output |= (uint32_t)(bytes[1] << 8);
  *output |= (uint32_t)(bytes[2] << 16);
  *output |= (uint32_t)(bytes[3] << 24);

  return 0;
}

int
_br_read_be_u32 (FILE *fptr, uint32_t *output)
{
  uint8_t bytes[4] = { 0 };
  for (size_t i = 0; i < 4; i++)
    {
      if (_br_read_u8 (fptr, &bytes[i]) != 0)
        return -1;
    }

  *output = bytes[3];
  *output |= (uint32_t)(bytes[2] << 8);
  *output |= (uint32_t)(bytes[1] << 16);
  *output |= (uint32_t)(bytes[0] << 24);

  return 0;
}

int
_br_read_le_be_u16 (FILE *fptr, uint16_t *output)
{
  if (_br_read_le_u16 (fptr, output) != 0)
    return -1;

  uint16_t value = 0;
  if (_br_read_be_u16 (fptr, &value) != 0)
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
_br_read_le_u16 (FILE *fptr, uint16_t *output)
{
  uint8_t bytes[2] = { 0 };
  for (size_t i = 0; i < 2; i++)
    {
      if (_br_read_u8 (fptr, &bytes[i]) != 0)
        return -1;
    }

  *output = bytes[0];
  *output |= (uint16_t)(bytes[1] << 8);

  return 0;
}

int
_br_read_be_u16 (FILE *fptr, uint16_t *output)
{
  uint8_t bytes[2] = { 0 };
  for (size_t i = 0; i < 2; i++)
    {
      if (_br_read_u8 (fptr, &bytes[i]) != 0)
        return -1;
    }

  *output = bytes[1];
  *output |= (uint16_t)(bytes[0] << 8);

  return 0;
}

int
_br_read_u8_array (FILE *fptr, uint8_t *output, size_t length)
{
  size_t bytes_read = fread (output, sizeof (uint8_t), length, fptr);
  if (bytes_read != sizeof (uint8_t) * length)
    {
      fprintf (stderr, "Error reading uint8 array of size %ld from file.\n",
               length);
      return -1;
    }

  return 0;
}
