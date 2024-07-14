/* clang-format off */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "errors.h"
#include "path_table.h"
#include "utils.h"
#include "volume_descriptor.h"
/* clang-format on */

#define BYTES_TO_READ 32

FILE *setup_extractor (int argc, char **argv);
void process_DAT_file (FILE *fptr);
void print_hex_data (unsigned char *buffer, const uint8_t BUFFER_LEN);
void process_volume_descriptor_header (FILE *fptr, volume_descriptor *vd);
void process_volume_descriptor_data (FILE *fptr, volume_descriptor_data *vdd);
void print_some_data_from_file (FILE *fptr);
void process_type_l_path_table (FILE *fptr, path_table *pt);

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

  volume_descriptor vd;
  process_volume_descriptor_header (fptr, &vd);

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

  // TODO: print the volume descriptor header/data to some file/log.

  // logical block size in big endian form
  uint16_t logical_block_size_be
      = change_endianness_uint16 (vd.data.logical_block_size);

  // Beginning of type-l path table
  fseek (fptr, logical_block_size_be * vd.data.type_l_path_table_location,
         SEEK_SET);

  path_table pt;
  if (create_path_table (&pt) != 0)
    {
      fclose (fptr);
      exit (1);
    }

  process_type_l_path_table (fptr, &pt);

  destroy_path_table (&pt);
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
  uint8_t descriptor_type = read_single_uint8 (fptr);

  fseek (fptr, 5, SEEK_CUR);

  uint8_t descriptor_ver = read_single_uint8 (fptr);

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

  vdd->volume_creation_date_and_time = read_dec_datetime (fptr);
  vdd->volume_modification_date_and_time = read_dec_datetime (fptr);
  vdd->volume_expiration_date_and_time = read_dec_datetime (fptr);
  vdd->volume_effective_date_and_time = read_dec_datetime (fptr);

  vdd->file_structure_version = read_single_uint8 (fptr);

  fseek (fptr, 1, SEEK_CUR); // Unused byte

  read_string (fptr, vdd->DAT_file_creation_software_identifier,
               DAT_FILE_CREATION_SOFTWARE_IDENTIFIER_LEN);

  fseek (fptr, 1, SEEK_CUR); // Unused byte

  read_string (fptr, vdd->DAT_file_creation_software_version_number,
               DAT_FILE_CREATION_SOFTWARE_VERSION_NUMBER_LEN);

  fseek (fptr, 1, SEEK_CUR); // Unused byte

  fseek (fptr, 0x1EC, SEEK_CUR); // Unknown data (492 bytes)

  fseek (fptr, 0x28D, SEEK_CUR); // Reserved by ISO
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

void
process_type_l_path_table (FILE *fptr, path_table *pt)
{
  print_some_data_from_file (fptr);

  uint8_t dir_identifier_length = read_single_uint8 (fptr);
  do
    {
      path_table_entry curr_entry;
      curr_entry.directory_identifier_length = dir_identifier_length;

      if (curr_entry.directory_identifier_length != 1)
        curr_entry.directory_identifier_length += 1;

      curr_entry.extended_attribute_record_length = read_single_uint8 (fptr);
      curr_entry.location_of_extent = read_little_endian_data_uint32_t (fptr);
      curr_entry.parent_directory_number
          = read_little_endian_data_uint16_t (fptr);

      curr_entry.directory_identifier = (char *)calloc (
          curr_entry.directory_identifier_length, sizeof (char));
      if (curr_entry.directory_identifier_length != 1)
        {
          size_t bytes_read
              = fread (curr_entry.directory_identifier, sizeof (char),
                       curr_entry.directory_identifier_length - 1, fptr);
          curr_entry
              .directory_identifier[curr_entry.directory_identifier_length - 1]
              = '\0';
          if (bytes_read
              != sizeof (char) * (curr_entry.directory_identifier_length - 1))
            {
              handle_fread_error (
                  fptr, bytes_read,
                  sizeof (char)
                      * (curr_entry.directory_identifier_length - 1));
            }
        }
      else
        {
          fseek (fptr, 1, SEEK_CUR);
        }

      add_entry_to_path_table (pt, &curr_entry);

      printf ("Current entry identifier: %s\n",
              curr_entry.directory_identifier);

      dir_identifier_length = read_single_uint8 (fptr);

      if (dir_identifier_length == 0) // handle padding field
        dir_identifier_length = read_single_uint8 (fptr);
      printf ("len: %02X\n", dir_identifier_length);
    }
  while (dir_identifier_length != 0);
}
