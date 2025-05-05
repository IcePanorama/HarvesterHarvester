#include "harvester_harvester/binary_reader.h"

int
_hhbr_read_str (FILE fptr[static 1], char output[static 1], size_t length)
{
  size_t bytes_read = fread (output, sizeof (char), length, fptr);
  if (bytes_read != (sizeof (char) * length))
    {
      fprintf (stderr, "Error reading string from file.\n");
      return -1;
    }

  return 0;
}

/** Returns: zero on success, non-zero on failure. */
static int
read_u8 (FILE *fptr, uint8_t *output)
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
_hhbr_read_le_u32 (FILE fptr[static 1], uint32_t output[static 1])
{
  uint8_t bytes[4] = { 0 };
  for (size_t i = 0; i < 4; i++)
    {
      if (read_u8 (fptr, &bytes[i]) != 0)
        return -1;
    }

  *output = bytes[0];
  *output |= (uint32_t)(bytes[1] << 8);
  *output |= (uint32_t)(bytes[2] << 16);
  *output |= (uint32_t)(bytes[3] << 24);

  return 0;
}
