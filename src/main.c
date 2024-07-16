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
#include "output.h"
#include "path_table.h"
#include "utils.h"
#include "volume_descriptor.h"
/* clang-format on */

/* function prototypes */
FILE *setup_extractor (char *filename);
void process_DAT_file (FILE *fptr);
void print_hex_data (unsigned char *buffer, const uint8_t BUFFER_LEN);
void process_volume_descriptor_header (FILE *fptr, volume_descriptor *vd);
void process_volume_descriptor_data (FILE *fptr, volume_descriptor_data *vdd);
void print_some_data_from_file (FILE *fptr);
void process_type_l_path_table (FILE *fptr, path_table *pt);
void process_directory (FILE *fptr, directory *d);
int8_t extract_file (FILE *fptr, directory_record *dr,
                     const char *directory_identifier);
int8_t extract_directory (FILE *fptr, const uint16_t BLOCK_SIZE,
                          const char *dir_identifier);
void handle_command_line_args (int argc, char **argv);
void handle_unknown_command_line_argument_error (char *arg);
/**********************/

static bool debug_mode = true;

int
main (int argc, char **argv)
{
  if (argc < 2)
    improper_usage_error ();

  if (argc >= 3)
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

  // Move to beginning of type-l path table
  fseek (fptr, LOGICAL_BLOCK_SIZE_BE * vd.data.type_l_path_table_location,
         SEEK_SET);

  path_table pt;
  if (create_path_table (&pt) != 0)
    {
      fclose (fptr);
      exit (1);
    }

  process_type_l_path_table (fptr, &pt);

  for (size_t i = pt.current_entry - 1; i > 0; --i)
    {
      path_table_entry curr_dir = pt.entries[i];
      // path_table_entry target_dir = curr_dir;

      // supports 10 levels of directories which is probably overkill.
      const uint32_t PATH_MAX_LEN
          = ((curr_dir.directory_identifier_length + 1) * 10)
            + (strlen (OUTPUT_DIR) + 1) + 1;
      char *path = calloc (PATH_MAX_LEN, sizeof (char));
      if (path == NULL)
        {
          perror ("ERROR: unable to calloc path string");
          destroy_path_table (&pt);
          exit (1);
        }

      strcat (path, curr_dir.directory_identifier);

      do
        {
          uint16_t index
              = change_endianness_uint16 (curr_dir.parent_directory_number);
          curr_dir
              = pt.entries[index - 1]; // parent_directory_number is 1-based

          prepend_path_string (path,
                               (const char *)curr_dir.directory_identifier);

          printf ("Dir ID: %s\n", curr_dir.directory_identifier);
          printf ("Path: %s\n", path);
        }
      while (curr_dir.parent_directory_number > 0x0100);

      create_output_directories (path);

      free (path);
    }

  destroy_path_table (&pt);
}

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

int8_t
extract_file (FILE *fptr, directory_record *dr,
              const char *directory_identifier)
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

  // +1 for the null terminator, +1 for `/` after directory identifier
  size_t filename_length = strlen (OUTPUT_DIR) + strlen (directory_identifier)
                           + strlen (actual_filename) + 2;

  char *output_filename = (char *)calloc (filename_length, sizeof (char));
  if (output_filename == NULL)
    {
      perror ("ERROR: unable to calloc `output_filename`.");
      return -1;
    }

  strcpy (output_filename, OUTPUT_DIR);
  strcat (output_filename, directory_identifier);

  if ((strcmp (directory_identifier, "") != 0)
      && (strcmp (directory_identifier, "\1") != 0))
    {
      strcat (output_filename, "/");
    }

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

int8_t
extract_directory (FILE *fptr, const uint16_t BLOCK_SIZE,
                   const char *dir_identifier)
{
  // 0xF00000 == 15 MiB
  const uint32_t DEBUG_FILE_SIZE_LIMIT = 0xF00000;

  directory dir;
  create_directory (&dir);
  process_directory (fptr, &dir);

  printf ("Extracting directory: %s\n", dir_identifier);

  for (size_t i = 0x0; i < dir.current_record; i++)
    {
      directory_record curr_file = dir.records[i];

      if (curr_file.file_flags.subdirectory)
        {
          if ((strcmp (curr_file.file_identifier, "") == 0)
              || (strcmp (curr_file.file_identifier, "\1") == 0))
            {
              continue;
            }

          char *folder_name = malloc (sizeof (char)
                                      * (strlen (curr_file.file_identifier)
                                         + strlen (OUTPUT_DIR) + 2));
          if (folder_name == NULL)
            {
              printf ("ERROR: unable to create folder %s.",
                      curr_file.file_identifier);
              return -1;
            }

          strcpy (folder_name, OUTPUT_DIR);
          strcat (folder_name, "/");
          strcat (folder_name, curr_file.file_identifier);

          // TODO: test me on Windows
          int status;
#ifdef _WIN32
          status = _mkdir (folder_name);
#else
          status = mkdir (folder_name, 0700);
#endif
          free (folder_name);
          if (status != 0)
            {
              printf ("ERROR: failed to create directory, %s\n",
                      curr_file.file_identifier);
              destroy_directory (&dir);
              return -1;
            }
          continue;
        }
      else if (debug_mode && curr_file.data_length > DEBUG_FILE_SIZE_LIMIT)
        {
          printf ("[DEBUG_MODE] Skipping file, %s.\n",
                  curr_file.file_identifier);
          continue;
        }

      fseek (fptr, curr_file.location_of_extent * BLOCK_SIZE, SEEK_SET);

      if (extract_file (fptr, &curr_file, dir_identifier) != 0)
        {
          destroy_directory (&dir);
          return -1;
        }
    }

  destroy_directory (&dir);
  return 0;
}

/*
 *  handle_command_line_args
 *
 *  TODO: add documentation
 */
void
handle_command_line_args (int argc, char **argv)
{
  for (int i = 2; i < argc; i++)
    {
      if (strcmp (argv[i], "--debug") == 0)
        {
          debug_mode = true;
        }
      else if (strcmp (argv[i], "--help") == 0)
        {
          // TODO: print out a list of possible command-line args
        }
      else
        {
          handle_unknown_command_line_argument_error (argv[i]);
        }
    }
}

/*
 *  handle_unknown_command_line_argument_error
 *
 *  TODO: add documentation.
 */
void
handle_unknown_command_line_argument_error (char *arg)
{
  printf ("ERROR: unknown command-line argument, %s.\n", arg);
  exit (1);
}

/*
 *  create_output_directories
 *
 *  TODO: add documentation
 */
/*
void
create_output_directories (char *path)
{
  char *tmp = calloc (strlen (path) + strlen (OUTPUT_DIR) + 2, sizeof (char));
  if (tmp == NULL)
    {
      perror ("ERROR: unable to calloc tmp string");
      exit (1);
    }
  strcpy (tmp, path);
  strcpy (path, OUTPUT_DIR);
  strcat (path, "/");
  strcat (path, tmp);
  free (tmp);

  tmp = calloc (strlen (path) + 2, sizeof (char));
  if (tmp == NULL)
    {
      perror ("ERROR: unable to calloc tmp string");
      exit (1);
    }
  strcpy (tmp, path);

  char *token = strtok (tmp, "/");

  char *dir = calloc (strlen (path) + 2, sizeof (char));
  if (dir == NULL)
    {
      perror ("ERROR: unable to calloc dir string");
      exit (1);
    }

  while (token != NULL)
    {
      strcat (dir, token);

      if (!directory_exists (dir))
        {
          // TODO: test me on Windows
          int status;
#ifdef _WIN32
          status = _mkdir (dir);
#else
          status = mkdir (dir, 0700);
#endif
          if (status != 0)
            {
              printf ("ERROR: failed to create directory, %s\n", path);
              free (dir);
              free (tmp);
              exit (1);
            }
        }

      strcat (dir, "/");
      token = strtok (NULL, "/");
    }

  free (dir);
  free (tmp);
}

bool
directory_exists (const char *dir)
{
#ifdef _WIN32
  struct _stat st;
  return (_stat (dir, &st) == 0 && (st.st_mode & _S_IFDIR));
#else
  struct stat st;
  return (stat (dir, &st) == 0 && S_ISDIR (st.st_mode));
#endif
}
*/
