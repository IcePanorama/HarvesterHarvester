#include "binary_reader.h"

int8_t
br_read_uint8_from_file (FILE *fptr, uint8_t *output)
{
  size_t bytes_read = fread (output, sizeof (uint8_t), 1, fptr);
  if (bytes_read != sizeof (uint8_t))
    {
      fprintf (stderr, "ERROR: Error reading uint8 from file.\n");
      return -1;
    }

  return 0;
}

int8_t
br_read_str_from_file (FILE *fptr, char *output, size_t length)
{
  size_t bytes_read = fread (output, sizeof (char), length, fptr);
  if (bytes_read != (sizeof (char) * length))
    {
      fprintf (stderr, "ERROR: Error reading string from file.\n");
      return -1;
    }

  return 0;
}

int8_t
br_read_le_be_uint32_from_file (FILE *fptr, uint32_t *output)
{
  if (br_read_le_uint32_from_file (fptr, output) != 0)
    return -1;

  uint32_t value = 0;
  if (br_read_be_uint32_from_file (fptr, &value) != 0)
    return -1;
  else if (*output != value)
    {
      fprintf (
          stderr,
          "ERROR: little endian 32-bit value read from file does not match "
          "the big endian value which follows it. Got %08X, expected %08X.\n",
          value, *output);
      return -1;
    }

  return 0;
}

int8_t
br_read_le_uint32_from_file (FILE *fptr, uint32_t *output)
{
  uint8_t bytes[4] = { 0 };
  for (size_t i = 0; i < 4; i++)
    {
      if (br_read_uint8_from_file (fptr, &bytes[i]) != 0)
        return -1;
    }

  *output = bytes[0];
  *output |= (uint32_t)(bytes[1] << 8);
  *output |= (uint32_t)(bytes[2] << 16);
  *output |= (uint32_t)(bytes[3] << 24);

  return 0;
}

int8_t
br_read_be_uint32_from_file (FILE *fptr, uint32_t *output)
{
  uint8_t bytes[4] = { 0 };
  for (size_t i = 0; i < 4; i++)
    {
      if (br_read_uint8_from_file (fptr, &bytes[i]) != 0)
        return -1;
    }

  *output = bytes[3];
  *output |= (uint32_t)(bytes[2] << 8);
  *output |= (uint32_t)(bytes[1] << 16);
  *output |= (uint32_t)(bytes[0] << 24);

  return 0;
}

int8_t
br_read_le_be_uint16_from_file (FILE *fptr, uint16_t *output)
{
  if (br_read_le_uint16_from_file (fptr, output) != 0)
    return -1;

  uint16_t value = 0;
  if (br_read_be_uint16_from_file (fptr, &value) != 0)
    return -1;
  else if (*output != value)
    {
      fprintf (
          stderr,
          "ERROR: little endian 16-bit value read from file does not match "
          "the big endian value which follows it. Got %04X, expected %04X.\n",
          value, *output);
      return -1;
    }

  return 0;
}

int8_t
br_read_le_uint16_from_file (FILE *fptr, uint16_t *output)
{
  uint8_t bytes[2] = { 0 };
  for (size_t i = 0; i < 2; i++)
    {
      if (br_read_uint8_from_file (fptr, &bytes[i]) != 0)
        return -1;
    }

  *output = bytes[0];
  *output |= (uint16_t)(bytes[1] << 8);

  return 0;
}

int8_t
br_read_be_uint16_from_file (FILE *fptr, uint16_t *output)
{
  uint8_t bytes[2] = { 0 };
  for (size_t i = 0; i < 2; i++)
    {
      if (br_read_uint8_from_file (fptr, &bytes[i]) != 0)
        return -1;
    }

  *output = bytes[1];
  *output |= (uint16_t)(bytes[0] << 8);

  return 0;
}

int8_t
br_read_uint8_array_from_file (FILE *fptr, uint8_t *output, size_t length)
{
  size_t bytes_read = fread (output, sizeof (uint8_t), length, fptr);
  if (bytes_read != sizeof (uint8_t) * length)
    {
      fprintf (stderr,
               "ERROR: Error reading uint8 array of size %ld from file.\n",
               length);
      return -1;
    }

  return 0;
}
