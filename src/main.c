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
