#include "errors.h"
#include "extractor.h"
#include "options.h"
#include "output.h"
#include "path_table.h"
#include "utils.h"
#include "volume_descriptor.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/* function prototypes */
static FILE *setup_extractor (char *filename);
static void process_DAT_file (FILE *fptr);
static void process_volume_descriptor_header (FILE *fptr,
                                              volume_descriptor *vd);
static void process_volume_descriptor_data (FILE *fptr,
                                            volume_descriptor_data *vdd);
static void process_type_l_path_table (FILE *fptr, path_table *pt);
static void batch_process_DAT_files (void);
/**********************/

int
main (int argc, char **argv)
{
  FILE *fptr;

  if (argc >= 2)
    handle_command_line_args (argc, argv);

  if (OPT_BATCH_PROCESS)
    {
      batch_process_DAT_files ();
    }
  else
    {
      fptr = setup_extractor (argv[argc - 1]);
      process_DAT_file (fptr);
      fclose (fptr);
    }
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

  size_t current_disk_name_length = strcspn (vd.data.volume_identifier, " ");
  current_disk_name = vd.data.volume_identifier;
  current_disk_name[current_disk_name_length] = '\0';
  print_volume_descriptor_data (&vd.data);

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

  char *path = calloc (strlen (OUTPUT_DIR) + current_disk_name_length + 2,
                       sizeof (char));
  if (path == NULL)
    {
      extract_directory (fptr, LOGICAL_BLOCK_SIZE_BE, OUTPUT_DIR);
      destroy_path_table (&pt);
      return;
    }

  strcpy (path, OUTPUT_DIR);
  strcat (path, "/");
  strcat (path, current_disk_name);
  extract_directory (fptr, LOGICAL_BLOCK_SIZE_BE, path);

  free (path);
  destroy_path_table (&pt);
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
 *  batch_process_DAT_files
 *
 *  Having this be super hard-coded is not ideal, but this should at least be
 *  cross-platform. I just threw this together in like 5 minutes. Definitely
 *  replace this in the future.
 *
 *  TODO: add better documentation.
 */
void
batch_process_DAT_files ()
{
  const uint8_t DAT_FILENAME_LEN = strlen ("HARVESTX.DAT");
  char *filename
      = calloc (strlen (OPT_INPUT_DIR) + DAT_FILENAME_LEN + 2, sizeof (char));
  if (filename == NULL)
    {
      perror ("ERROR: unable to calloc filename for batch processing");
      exit (1);
    }
  FILE *fptr;

  strcpy (filename, OPT_INPUT_DIR);
  strcat (filename, "/");
  strcat (filename, "HARVEST.DAT");
  fptr = setup_extractor (filename);
  process_DAT_file (fptr);
  fclose (fptr);

  strcpy (filename, OPT_INPUT_DIR);
  strcat (filename, "/");
  strcat (filename, "HARVEST3.DAT");
  fptr = setup_extractor (filename);
  process_DAT_file (fptr);
  fclose (fptr);

  strcpy (filename, OPT_INPUT_DIR);
  strcat (filename, "/");
  strcat (filename, "HARVEST4.DAT");
  fptr = setup_extractor (filename);
  process_DAT_file (fptr);
  fclose (fptr);

  free (filename);
}
