//  Copyright (C) 2024  IcePanorama
//  This file is a part of HarvesterHarvester.
//  HarvesterHarvester is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by the
//  Free Software Foundation, either version 3 of the License, or (at your
//  option) any later version.
//  This program is distributed in the hope that it will be useful, but WITHOUT
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//  more details.
//
//  You should have received a copy of the GNU General Public License along
//  with this program.  If not, see <https://www.gnu.org/licenses/>.
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
extract_file_using_dir_record (FILE *fptr, struct directory_record *dr,
                               const char *path)
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
       *  should be fine.
       */
      actual_filename = dr->file_identifier;
    }

  printf ("[HarvesterHarvester]Extracting file: %s\n", actual_filename);

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
extract_directory (FILE *fptr, const uint16_t block_size, const char *path)
{
  directory dir;
  create_directory (&dir);
  process_directory (fptr, &dir);

  printf ("[HarvesterHarvester]Extracting directory: %s\n", path);

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
          printf ("[HarvesterHarvester][DEBUG_MODE] Skipping file, %s.\n",
                  curr_file.file_identifier);
          continue;
        }

      fseek (fptr, curr_file.location_of_extent * block_size, SEEK_SET);

      if (extract_file_using_dir_record (fptr, &curr_file, path) != 0)
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
                                                    uint16_t block_size,
                                                    path_table *pt)
{
  for (size_t i = pt->current_entry - 1; i > 0; --i)
    {
      path_table_entry curr_dir = pt->entries[i];
      path_table_entry target_dir = curr_dir;

      // supports 10 levels of directories which is probably overkill.
      const uint32_t PATH_MAX_LEN
          = ((curr_dir.directory_identifier_length + 1) * 10)
            + (strlen (OP_OUTPUT_DIR) + 1) + (strlen (CURRENT_DISK_NAME) + 1)
            + 1;
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

      fseek (fptr, block_size * target_dir.location_of_extent, SEEK_SET);

      if (extract_directory (fptr, block_size, path) != 0)
        return -1;

      free (path);
    }

  return 0;
}

int8_t
extract_file_using_idx_entry (FILE *fptr, index_entry *idx, const char *path)
{
  printf ("[HarvesterHarvester]Extracting file: %s\n", path);

  FILE *output_file = fopen (path, "wb");
  if (output_file == NULL)
    {
      char tmp[256] = { 0 };
      strncpy (tmp, path, strlen (path) - strlen (idx->filename));

      if (create_new_output_directory (tmp) != 0)
        return -1;

      output_file = fopen (path, "wb");
      if (output_file == NULL)
        {
          fprintf (
              stderr,
              "[HarvesterHarvester]ERROR: Error opening output file, %s.\n",
              path);
          return HH_FOPEN_ERROR;
        }
    }

  fseek (fptr, idx->start, SEEK_SET);

  for (uint32_t i = 0x0; i < idx->size; i++)
    {
      uint8_t byte;
      if (read_single_uint8 (fptr, &byte) != 0)
        {
          fprintf (
              stderr,
              "[HarvesterHarvester]ERROR: couldn't read byte, quitting.\n");
          fclose (output_file);
          return -1;
        }

      fwrite (&byte, sizeof (uint8_t), 1, output_file);
    }

  fclose (output_file);
  return 0;
}

int8_t
extract_index_file (index_file *idx, const char *idx_path,
                    const char *dat_path)
{
  FILE *dat_file = fopen (dat_path, "rb");
  if (dat_file == NULL)
    {
      fprintf (stderr,
               "[HarvesterHarvester]ERROR: error opening dat file, %s.\n",
               dat_path);
      return HH_FOPEN_ERROR;
    }

  for (size_t i = 0; i < idx->current_index; i++)
    {
      char output_path[256] = { 0 };

      /**
       *  Gives you the `OP_OUTPUT_DIR/DISK#/` part of the path.
       *  10 = len("INDEX.001")
       */
      strncpy (output_path, idx_path, strlen (idx_path) - 10);
      strcat (output_path, idx->indicies[i].full_path);

      if (extract_file_using_idx_entry (dat_file, &idx->indicies[i],
                                        output_path)
          != 0)
        {
          fclose (dat_file);
          return -1;
        }
    }

  fclose (dat_file);
  return 0;
}
