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

// For directory handling
#ifdef _WIN32
#include <Windows.h>
#else
#include <dirent.h>
#endif

static FILE *setup_extractor (char *filename);
static void process_DAT_file (FILE *fptr);
static int batch_process_DAT_files (void);

int
main (int argc, char **argv)
{
  FILE *fptr;

  if (argc >= 2)
    handle_command_line_args (argc, argv);

  if (OP_BATCH_PROCESS)
    {
      if (batch_process_DAT_files () != 0)
        {
          exit (1);
        }
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

#ifdef _WIN32
  OP_PATH_SEPARATOR = '\\';
#else
  OP_PATH_SEPARATOR = '/';
#endif

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
  OP_CURRENT_DISK_NAME = vd.data.volume_identifier;
  OP_CURRENT_DISK_NAME[current_disk_name_length] = '\0';

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
  strcat (path, OP_CURRENT_DISK_NAME);
  extract_directory (fptr, LOGICAL_BLOCK_SIZE_BE, path);

  free (path);
  destroy_path_table (&pt);
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
int
batch_process_DAT_files ()
{
  const uint8_t DAT_FILENAME_LEN = strlen ("HARVESTX.DAT");
  char *filename;

#ifdef _WIN32
  WIN32_FIND_DATAA file_data;
  HANDLE hFind;
  char search_path[MAX_PATH];
  strcpy (search_path, OP_INPUT_DIR);
  strcat (search_path, "\\*");

  hFind = FindFirstFileA (search_path, &file_data);
  if (hFind == INVALID_HANDLE_VALUE)
    {
      fprintf (stderr, "ERROR: Error opening input directory, %s.\n",
               OP_INPUT_DIR);
      return -1;
    }

  do
    {
      if (strcmp (file_data.cFileName, ".") == 0
          || strcmp (file_data.cFileName, "..") == 0
          || !is_string_dat_file (file_data.cFileName))
        {
          continue;
        }

      filename = calloc (strlen (OP_INPUT_DIR) + DAT_FILENAME_LEN + 2,
                         sizeof (char));
      if (filename == NULL)
        {
          perror ("ERROR: unable to calloc string for filename.");
          exit (1);
        }

      strcpy (filename, OP_INPUT_DIR);
      strcat (filename, &OP_PATH_SEPARATOR);
      strcat (filename, file_data.cFileName);

      FILE *fptr = setup_extractor (filename);
      process_DAT_file (fptr);

      fclose (fptr);
      free (filename);
    }
  while (FindNextFileA (hFind, &file_data) != 0);

  FindClose (hFind);
#else
  struct dirent *entry;
  DIR *dir;
  dir = opendir (OP_INPUT_DIR);
  if (dir == NULL)
    {
      fprintf (stderr, "ERROR: Error opening input directory, %s.\n",
               OP_INPUT_DIR);
      return -1;
    }

  while ((entry = readdir (dir)) != NULL)
    {
      if (strcmp (entry->d_name, ".") == 0 || strcmp (entry->d_name, "..") == 0
          || !is_string_dat_file (entry->d_name))
        {
          continue;
        }

      filename = calloc (strlen (OP_INPUT_DIR) + DAT_FILENAME_LEN + 2,
                         sizeof (char));
      if (filename == NULL)
        {
          perror ("ERROR: unable to calloc string for filename.");
          exit (1);
        }

      strcpy (filename, OP_INPUT_DIR);
      strcat (filename, &OP_PATH_SEPARATOR);
      strcat (filename, entry->d_name);

      FILE *fptr = setup_extractor (filename);
      process_DAT_file (fptr);

      fclose (fptr);
      free (filename);
    }

  closedir (dir);
  return 0;
#endif
}
