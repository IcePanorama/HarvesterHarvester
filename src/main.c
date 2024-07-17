/* clang-format off */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "directory.h"
#include "errors.h"
#include "file_flags.h"
#include "options.h"
#include "output.h"
#include "path_table.h"
#include "utils.h"
#include "volume_descriptor.h"
/* clang-format on */

/* function prototypes */
FILE *setup_extractor (char *filename);
void process_DAT_file (FILE *fptr);
void process_volume_descriptor_header (FILE *fptr, volume_descriptor *vd);
void process_volume_descriptor_data (FILE *fptr, volume_descriptor_data *vdd);
void process_type_l_path_table (FILE *fptr, path_table *pt);
void process_directory (FILE *fptr, directory *d);
int8_t extract_file (FILE *fptr, directory_record *dr, const char *path);
int8_t extract_directory (FILE *fptr, const uint16_t BLOCK_SIZE,
                          const char *path);
void create_directories_and_extract_data_from_path_file (FILE *fptr,
                                                         uint16_t BLOCK_SIZE,
                                                         path_table *pt);
/**********************/

int
main (int argc, char **argv)
{
  if (argc < 2)
    improper_usage_error ();
  else
    handle_command_line_args (argc, argv);

  FILE *fptr = setup_extractor (argv[1]);

  process_DAT_file (fptr);

  fclose (fptr);
  return 0;
}

/*
 *  setup_extractor
 *
 *  Creates a `FILE *` for the given `filename`, handling error messages as
 *  needed.
 *  In the future, this function will be used to search the `dat files`
 *  directory and automatically prepare the necessary pointers for whatever
 *  files it is passed. Additionally, this function will also be used to handle
 *  commandline arguements from users.
 *
 *  param:
 *    filename : char * - the name/path of the `HARVEST(X).DAT` file.
 *  returns:
 *    FILE * - a pointer to the given file, if it exist.
 */
FILE *
setup_extractor (char *filename)
{
  FILE *fptr = fopen (filename, "rb");
  if (fptr == NULL)
    {
      fopen_error (filename);
      // fopen_error has an `exit(1)` in it, but not including this next line
      // produces a `use of NULL ‘fptr’ where non-null expected` Werror.
      exit (1);
    }

  return fptr;
}

/*
 *  process_DAT_file
 *
 *  TODO: write some better documentation for this.
 *  In the meantime, please see the comments of all the functions called within
 *  for more details.
 */
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

  // logical block size, in big endian form
  const uint16_t LOGICAL_BLOCK_SIZE_BE
      = change_endianness_uint16 (vd.data.logical_block_size);

  // move to beginning of type-l path table
  fseek (fptr, LOGICAL_BLOCK_SIZE_BE * vd.data.type_l_path_table_location,
         SEEK_SET);

  path_table pt;
  if (create_path_table (&pt) != 0)
    {
      fclose (fptr);
      exit (1);
    }

  process_type_l_path_table (fptr, &pt);

  create_directories_and_extract_data_from_path_file (
      fptr, LOGICAL_BLOCK_SIZE_BE, &pt);

  // handle root directory
  fseek (fptr, LOGICAL_BLOCK_SIZE_BE * pt.entries[0].location_of_extent,
         SEEK_SET);
  extract_directory (fptr, LOGICAL_BLOCK_SIZE_BE, OUTPUT_DIR);

  destroy_path_table (&pt);

  // Volume identifier has the disk name
  // print_volume_descriptor_data (&vd.data);
}

/*
 *  process_volume_descriptor_header
 *
 *  TODO: write some better documentation for this.
 *  In the meantime, see: https://wiki.osdev.org/ISO_9660#Volume_Descriptors
 */
void
process_volume_descriptor_header (FILE *fptr, volume_descriptor *vd)
{
  uint8_t descriptor_type = read_single_uint8 (fptr);

  fseek (fptr, 5, SEEK_CUR);

  uint8_t descriptor_ver = read_single_uint8 (fptr);

  create_volume_descriptor (vd, descriptor_type, descriptor_ver);
}

/*
 *  process_volume_descriptor_data
 *
 *  TODO: write some better documentation for this.
 *  In the meantime, see:
 *  https://wiki.osdev.org/ISO_9660#The_Primary_Volume_Descriptor
 */
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

/*
 *  process_type_l_path_table
 *
 *  TODO: write some better documentation.
 *  In the meantime, see: https://wiki.osdev.org/ISO_9660#The_Path_Table
 */
void
process_type_l_path_table (FILE *fptr, path_table *pt)
{
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

      dir_identifier_length = read_single_uint8 (fptr);

      if (dir_identifier_length == 0) // handle padding field
        dir_identifier_length = read_single_uint8 (fptr);
    }
  while (dir_identifier_length != 0);
}

/*
 *  process_directory
 *
 *  TODO: write some better documentation.
 *  In the meantime, see: https://wiki.osdev.org/ISO_9660#Directories
 */
void
process_directory (FILE *fptr, directory *d)
{
  uint8_t single_byte = read_single_uint8 (fptr);
  do
    {
      directory_record dr;
      dr.record_length = single_byte;
      dr.extended_attribute_record_length = read_single_uint8 (fptr);
      dr.location_of_extent = read_both_endian_data_uint32 (fptr);
      dr.data_length = read_both_endian_data_uint32 (fptr);
      dr.recording_datetime = read_dir_datetime (fptr);
      dr.file_flags = create_file_flags ();
      read_file_flags (fptr, &dr.file_flags);
      dr.file_unit_size = read_single_uint8 (fptr);
      dr.interleave_gap_size = read_single_uint8 (fptr);
      dr.volume_sequence_number = read_both_endian_data_uint16 (fptr);
      dr.file_identifier_length = read_single_uint8 (fptr) + 1;

      dr.file_identifier
          = (char *)calloc (dr.file_identifier_length, sizeof (char));
      size_t bytes_read = fread (dr.file_identifier, sizeof (char),
                                 dr.file_identifier_length - 1, fptr);
      dr.file_identifier[dr.file_identifier_length - 1] = '\0';
      if (bytes_read != sizeof (char) * (dr.file_identifier_length - 1))
        {
          handle_fread_error (fptr, bytes_read,
                              sizeof (char) * (dr.file_identifier_length - 1));
        }

      add_record_to_directory (d, &dr);

      if (dr.file_identifier_length % 2 != 0) // handle padding field
        fseek (fptr, 1, SEEK_CUR);

      single_byte = read_single_uint8 (fptr);
    }
  while (single_byte != 0);
}

/*
 *  extract_file
 *
 *  TODO: add documentation
 */
int8_t
extract_file (FILE *fptr, directory_record *dr, const char *path)
{
  /*
   *  the `file_identifier` terminates with a `;` character followed by the
   *  file ID number in ASCII coded decimal (`1`).
   *  See: https://wiki.osdev.org/ISO_9660#Directories
   */
  char *actual_filename = strtok (dr->file_identifier, (const char *)";");
  if (actual_filename == NULL)
    {
      /*
       *  Just use the default/existing filename.
       *  It'll be incorrect, but probably not worth stoping execution over.
       *  Users could just manually remove the `;1` part; the data itself
       * should be fine.
       */
      actual_filename = dr->file_identifier;
    }

  printf ("Extracting file: %s\n", actual_filename);

  // +1 for the null terminator, +1 for `/` between dir and filename
  size_t filename_length = strlen (path) + strlen (actual_filename) + 2;

  char *output_filename = (char *)calloc (filename_length, sizeof (char));
  if (output_filename == NULL)
    {
      perror ("ERROR: unable to calloc `output_filename`.");
      return -1;
    }

  strcpy (output_filename, path);
  strcat (output_filename, (const char *)"/");
  strcat (output_filename, actual_filename);

  FILE *output_file = fopen (output_filename, "wb");
  if (output_file == NULL)
    {
      perror ("ERROR: unable to open output file.");
      free (output_filename);
      return -1;
    }

  // `j` must be in hex, otherwise `data_length` can be treated as an int value
  for (uint32_t j = 0x0; j < dr->data_length; j++)
    {
      uint8_t byte = read_single_uint8 (fptr);
      fwrite (&byte, sizeof (uint8_t), 1, output_file);
    }

  fclose (output_file);
  free (output_filename);

  return 0;
}

/*
 *  extract_directory
 *
 *  TODO: add documentation
 */
int8_t
extract_directory (FILE *fptr, const uint16_t BLOCK_SIZE, const char *path)
{
  // 0xF00000 == 15 MiB
  const uint32_t DEBUG_FILE_SIZE_LIMIT = 0xF00000;

  directory dir;
  create_directory (&dir);
  process_directory (fptr, &dir);

  printf ("Extracting directory: %s\n", path);

  for (size_t i = 0x0; i < dir.current_record; i++)
    {
      directory_record curr_file = dir.records[i];

      if (curr_file.file_flags.subdirectory)
        {
          continue;
        }
      else if (debug_mode && curr_file.data_length > DEBUG_FILE_SIZE_LIMIT)
        {
          printf ("[DEBUG_MODE] Skipping file, %s.\n",
                  curr_file.file_identifier);
          continue;
        }

      fseek (fptr, curr_file.location_of_extent * BLOCK_SIZE, SEEK_SET);

      if (extract_file (fptr, &curr_file, path) != 0)
        {
          destroy_directory (&dir);
          return -1;
        }
    }

  destroy_directory (&dir);
  return 0;
}

/*
 *  create_directories_and_extract_data_from_path_file
 *
 *  TODO: add documentation
 */
void
create_directories_and_extract_data_from_path_file (FILE *fptr,
                                                    uint16_t BLOCK_SIZE,
                                                    path_table *pt)
{
  for (size_t i = pt->current_entry - 1; i > 0; --i)
    {
      path_table_entry curr_dir = pt->entries[i];
      path_table_entry target_dir = curr_dir;

      // supports 10 levels of directories which is probably overkill.
      const uint32_t PATH_MAX_LEN
          = ((curr_dir.directory_identifier_length + 1) * 10)
            + (strlen (OUTPUT_DIR) + 1) + 1;
      char *path = calloc (PATH_MAX_LEN, sizeof (char));
      if (path == NULL)
        {
          perror ("ERROR: unable to calloc path string");
          destroy_path_table (pt);
          exit (1);
        }

      strcat (path, curr_dir.directory_identifier);

      do
        {
          uint16_t index
              = change_endianness_uint16 (curr_dir.parent_directory_number);
          // parent_directory_number is 1-based
          curr_dir = pt->entries[index - 1];

          prepend_path_string (path,
                               (const char *)curr_dir.directory_identifier);
        }
      while (curr_dir.parent_directory_number > 0x0100);

      create_output_directory (path);

      fseek (fptr, BLOCK_SIZE * target_dir.location_of_extent, SEEK_SET);

      extract_directory (fptr, BLOCK_SIZE, path);

      free (path);
    }
}
