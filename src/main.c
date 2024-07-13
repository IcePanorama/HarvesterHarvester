/* clang-format off */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "volume_descriptor.h"
/* clang-format on */

#define BYTES_TO_READ 32

FILE *setup_extractor (int argc, char **argv);
void improper_usage_error (void);
void fopen_error (char *filename);
void process_DAT_file (FILE *fptr);
void print_hex_data (unsigned char *buffer, const uint8_t BUFFER_LEN);
void process_volume_descriptor_header (FILE *fptr, volume_descriptor *vd);

int
main (int argc, char **argv)
{
  FILE *fptr = setup_extractor (argc, argv);

  process_DAT_file (fptr);

  fclose (fptr);
  return 0;
}

FILE *
setup_extractor (int argc, char **argv)
{
  if (argc < 2)
    improper_usage_error ();

  FILE *fptr = fopen (argv[1], "rb");
  if (fptr == NULL)
    fopen_error (argv[1]);

  return fptr;
}

void
improper_usage_error (void)
{
  const char *EXECUTABLE_NAME = "main";

  puts ("ERROR: Improper usage!");
  printf ("try: $./%s HARVEST.DAT\n", EXECUTABLE_NAME);

  exit (1);
}

void
fopen_error (char *filename)
{
  printf ("ERROR: Unable to open file, %s.\n", filename);
  exit (1);
}

void
process_DAT_file (FILE *fptr)
{
  unsigned char buffer[BYTES_TO_READ];

  /*
   * First 32k (up to 0x8000) stores the `system area` of the disk.
   * Unused by HARVEST.DAT, HARVEST3.DAT, HARVEST4.DAT.
   * See: https://wiki.osdev.org/ISO_9660#System_Area
   */
  fseek (fptr, 0x8000, SEEK_SET);

  fread (buffer, sizeof (buffer), 1, fptr);
  print_hex_data (buffer, BYTES_TO_READ);

  fseek (fptr, -sizeof (buffer), SEEK_CUR);

  volume_descriptor vd;
  process_volume_descriptor_header (fptr, &vd);
  print_volume_descriptor_header (&vd);

  if (vd.type_code != 0x01)
    {
      puts ("Error: Unexpected volume descriptor type code.");
      printf ("\tExpected %02x, got %02x.\n", 0x01, vd.type_code);
      puts ("Note: Extracting files from HARVEST2.DAT is currently unsupported.");
      fclose (fptr);
      exit (1);
    }
}

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

void
process_volume_descriptor_header (FILE *fptr, volume_descriptor *vd)
{
  uint8_t descriptor_type;
  fread (&descriptor_type, sizeof (uint8_t), 1, fptr);

  // Identifier is always CD001, so we just skip it.
  // See: https://wiki.osdev.org/ISO_9660#Volume_Descriptors
  fseek (fptr, sizeof (char) * 5, SEEK_CUR);

  uint8_t descriptor_ver;
  fread (&descriptor_ver, sizeof (uint8_t), 1, fptr);

  create_volume_descriptor (vd, descriptor_type, descriptor_ver);
}
