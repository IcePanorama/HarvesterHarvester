#include "log.h"
#include "errors.h"
#include <stdio.h>

void print_hex_data (unsigned char *buffer, const uint8_t BUFFER_LEN);

// TODO: move this to a "logging" function or something like that.
/*
 *  print_hex_data
 *
 *  Outputs hex data from a given `buffer` to stdout, formatting said data to
 *  add spaces between bytes, tabs after after every four bytes, and a new line
 *  after every 16 bytes.
 *
 *  param:
 *    buffer : unsigned char * - a buffer containing hex data.
 *    BUFFER_LEN : const uint8_t - the size of the data buffer.
 */
void
print_hex_data (unsigned char *buffer, const uint8_t BUFFER_LEN)
{
  for (uint8_t i = 0; i < BUFFER_LEN; i++)
    {
      printf (" %02x", buffer[i]);
      if ((i != 0) && (i % 4 == 3))
        {
          printf ("\t");
        }
      if ((i != 0) && (i % 16 == 15))
        {
          puts ("");
        }
    }
  puts ("");
}

/*
 *  print_some_data_from_file
 *
 *  Prints out the next `BYTES_TO_READ` many bytes to the stdout, formatted
 *  properly in order to be easy to read. Useful for debugging.
 */
#define BYTES_TO_READ 32
void
print_some_data_from_file (FILE *fptr)
{
  unsigned char buffer[BYTES_TO_READ];
  size_t bytes_read = fread (buffer, sizeof (buffer[0]), BYTES_TO_READ, fptr);
  if (bytes_read != BYTES_TO_READ)
    {
      handle_fread_error (fptr, bytes_read, BYTES_TO_READ);
    }
  print_hex_data (buffer, BYTES_TO_READ);
  fseek (fptr, -sizeof (buffer), SEEK_CUR);
}
