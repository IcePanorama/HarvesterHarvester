#include "extractor.h"
#include "data_reader.h"
#include "errors.h"
#include "options.h"
#include "output.h"
#include "utils.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

int8_t
extract_file (FILE *fptr, directory_record *dr, const char *path)
{
  /**
   *  the `file_identifier` terminates with a `;` character followed by the
   *  file ID number in ASCII coded decimal (`1`).
   *  See: https://wiki.osdev.org/ISO_9660#Directories
   */
  char *actual_filename = strtok (dr->file_identifier, (const char *)";");
  if (actual_filename == NULL)
    {
      /**
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
      fprintf (stderr, CALLOC_FAILED_ERR_MSG_FMT, filename_length);
      return HH_MEM_ALLOC_ERROR;
    }

  strcpy (output_filename, path);
  strcat (output_filename, &OP_PATH_SEPARATOR);
  strcat (output_filename, actual_filename);

  FILE *output_file = fopen (output_filename, "wb");
  if (output_file == NULL)
    {
      fprintf (stderr, FOPEN_FAILED_ERR_MSG_FMT, output_filename);
      free (output_filename);
      return HH_FOPEN_ERROR;
    }

  // `j` must be in hex, otherwise `data_length` can be treated as an int value
  for (uint32_t j = 0x0; j < dr->data_length; j++)
    {
      uint8_t byte;
      if (read_single_uint8 (fptr, &byte) != 0)
        {
          fclose (output_file);
          free (output_filename);
          return HH_FREAD_ERROR;
        }

      fwrite (&byte, sizeof (uint8_t), 1, output_file);
    }

  fclose (output_file);
  free (output_filename);
  return 0;
}

int8_t
extract_directory (FILE *fptr, const uint16_t BLOCK_SIZE, const char *path)
{
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
      else if (OP_DEBUG_MODE
               && curr_file.data_length > OP_DEBUG_FILE_SIZE_LIMIT)
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

int8_t
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
            + (strlen (OP_OUTPUT_DIR) + 1)
            + (strlen (OP_CURRENT_DISK_NAME) + 1) + 1;
      char *path = calloc (PATH_MAX_LEN, sizeof (char));
      if (path == NULL)
        {
          fprintf (stderr, CALLOC_FAILED_ERR_MSG_FMT, PATH_MAX_LEN);
          return HH_MEM_ALLOC_ERROR;
        }

      strcat (path, curr_dir.directory_identifier);

      do
        {
          uint16_t index
              = change_endianness_uint16 (curr_dir.parent_directory_number);

          // parent_directory_number is 1-based
          curr_dir = pt->entries[index - 1];

          if (prepend_path_string (path,
                                   (const char *)curr_dir.directory_identifier)
              != 0)
            {
              free (path);
              return -1;
            }
        }
      while (curr_dir.parent_directory_number > 0x0100);

      if (create_output_directory (path) != 0)
        return -1;

      fseek (fptr, BLOCK_SIZE * target_dir.location_of_extent, SEEK_SET);

      if (extract_directory (fptr, BLOCK_SIZE, path) != 0)
        return -1;

      free (path);
    }

  return 0;
}
