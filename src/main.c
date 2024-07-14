/* clang-format off */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "errors.h"
#include "volume_descriptor.h"
#include "utils.h"
#include "dec_datetime.h"
/* clang-format on */

#define BYTES_TO_READ 32

FILE *setup_extractor (int argc, char **argv);
void process_DAT_file (FILE *fptr);
void print_hex_data (unsigned char *buffer, const uint8_t BUFFER_LEN);
void process_volume_descriptor_header (FILE *fptr, volume_descriptor *vd);
void process_volume_descriptor_data (FILE *fptr, volume_descriptor_data *vdd);
void print_some_data_from_file (FILE *fptr);

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
    {
      fopen_error (argv[1]);
      // fopen_error has an `exit(1)` in it, but not including this next line
      // produces a `use of NULL ‘fptr’ where non-null expected` Werror.
      exit (1);
    }

  return fptr;
}

void
process_DAT_file (FILE *fptr)
{
  /*
   * First 32k (up to 0x8000) stores the `system area` of the disk.
   * Unused by HARVEST.DAT, HARVEST3.DAT, HARVEST4.DAT.
   * See: https://wiki.osdev.org/ISO_9660#System_Area
   */
  fseek (fptr, 0x8000, SEEK_SET);

  print_some_data_from_file (fptr);

  volume_descriptor vd;
  process_volume_descriptor_header (fptr, &vd);
  print_volume_descriptor_header (&vd);

  // Verify that this is a primary volume descriptor
  if (vd.type_code != 0x01)
    {
      puts ("Error: Unexpected volume descriptor type code.");
      printf ("\tExpected %02x, got %02x.\n", 0x01, vd.type_code);
      puts ("Note: Extracting files from HARVEST2.DAT is currently "
            "unsupported.");
      fclose (fptr);
      exit (1);
    }

  process_volume_descriptor_data (fptr, &vd.data);
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
  size_t bytes_read = fread (&descriptor_type, sizeof (uint8_t), 1, fptr);
  if (bytes_read != sizeof (uint8_t))
    {
      handle_fread_error (fptr, bytes_read, sizeof (uint8_t));
    }

  // Identifier is always CD001, so we just skip it.
  // See: https://wiki.osdev.org/ISO_9660#Volume_Descriptors
  fseek (fptr, 5, SEEK_CUR);

  uint8_t descriptor_ver;
  bytes_read = fread (&descriptor_ver, sizeof (uint8_t), 1, fptr);
  if (bytes_read != sizeof (uint8_t))
    {
      handle_fread_error (fptr, bytes_read, sizeof (uint8_t));
    }

  create_volume_descriptor (vd, descriptor_type, descriptor_ver);
}

void
process_volume_descriptor_data (FILE *fptr, volume_descriptor_data *vdd)
{
  fseek (fptr, 1, SEEK_CUR); // Unused byte

  read_string (fptr, vdd->system_identifier, SYSTEM_IDENTIFIER_LEN);
  read_string (fptr, vdd->volume_identifier, VOLUME_IDENTIFIER_LEN);

  fseek (fptr, 8, SEEK_CUR); // Unused field

  vdd->volume_space_size = read_both_endian_data_uint32 (fptr);

  fseek (fptr, 32, SEEK_CUR); // Unused field

  vdd->volume_set_size = read_both_endian_data_uint16 (fptr);
  vdd->volume_sequence_number = read_both_endian_data_uint16 (fptr);
  vdd->logical_block_size = read_both_endian_data_uint16 (fptr);
  vdd->path_table_size = read_both_endian_data_uint32 (fptr);
  vdd->type_l_path_table_location = read_little_endian_data_uint32_t (fptr);
  vdd->optional_type_l_path_table_location
      = read_little_endian_data_uint32_t (fptr);
  vdd->type_m_path_table_location = read_little_endian_data_uint32_t (fptr);
  vdd->optional_type_m_path_table_location
      = read_little_endian_data_uint32_t (fptr);

  read_array_uint8 (fptr, vdd->root_directory_entry, ROOT_DIRECTORY_ENTRY_LEN);

  read_string (fptr, vdd->volume_set_identifier, VOLUME_SET_IDENTIFIER_LEN);
  read_string (fptr, vdd->publisher_identifier, PUBLISHER_IDENTIFIER_LEN);
  read_string (fptr, vdd->data_preparer_identifier,
               DATA_PREPARER_IDENTIFIER_LEN);
  read_string (fptr, vdd->application_identifier, APPLICATION_IDENTIFIER_LEN);
  read_string (fptr, vdd->copyright_file_identifier,
               COPYRIGHT_FILE_IDENTIFIER_LEN);
  read_string (fptr, vdd->abstract_file_identifier,
               ABSTRACT_FILE_IDENTIFIER_LEN);
  read_string (fptr, vdd->bibliographic_file_identifier,
               BIBLIOGRAPHIC_FILE_IDENTIFIER_LEN);

  printf ("System identifier: %s\n", vdd->system_identifier);
  printf ("Volume identifier: %s\n", vdd->volume_identifier);
  printf ("Vol space size: %08X\n", vdd->volume_space_size);
  printf ("Vol set size: %04X\n", vdd->volume_set_size);
  printf ("Vol sequence size: %04X\n", vdd->volume_sequence_number);
  printf ("Logical block size: %04X\n", vdd->logical_block_size);
  printf ("Path table size: %08X\n", vdd->path_table_size);
  printf ("Type-L path table location: %08X\n",
          vdd->type_l_path_table_location);
  printf ("Optional Type-L path table location: %08X\n",
          vdd->optional_type_l_path_table_location);
  printf ("Type-M path table location: %08X\n",
          vdd->type_m_path_table_location);
  printf ("Optional Type-M path table location: %08X\n",
          vdd->optional_type_m_path_table_location);
  printf ("Volume set identifier: %s\n", vdd->volume_set_identifier);
  printf ("Publisher identifier: %s\n", vdd->publisher_identifier);
  printf ("Data preparer identifier: %s\n", vdd->data_preparer_identifier);
  printf ("Application identifier: %s\n", vdd->application_identifier);
  printf ("Copyright file identifier: %s\n", vdd->copyright_file_identifier);
  printf ("Abstract file identifier: %s\n", vdd->abstract_file_identifier);
  printf ("Bibliographic file identifier: %s\n",
          vdd->bibliographic_file_identifier);

  print_some_data_from_file (fptr);
  vdd->volume_creation_date_and_time = read_dec_datetime (fptr);
  printf ("Volume creation date and time: ");
  print_dec_datetime (vdd->volume_creation_date_and_time);
}

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
