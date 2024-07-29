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
#include "hh.h"
#include "errors.h"
#include "extractor.h"
#include "index_file.h"
#include "options.h"
#include "path_table.h"
#include "utils.h"
#include "volume_descriptor.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#endif

int
main (int argc, char **argv)
{
  FILE *fptr = NULL;

  if (argc >= 2)
    handle_command_line_args (argc, argv);

  if (OP_BATCH_PROCESS && !OP_SKIP_DAT_PROCESSING)
    {
      if (batch_process_DAT_files () != 0)
        exit (1);
    }
  else if (!OP_SKIP_DAT_PROCESSING)
    {
      if (setup_extractor (&fptr, argv[argc - 1]) != 0)
        exit (1);

      if (process_DAT_file (fptr) != 0)
        exit (1);

      fclose (fptr);
    }

  /*
   * TODO: Next steps:
   * [] navigate to each output dir ("DISK1", "DISK2", etc.)
   *    [] Path is just `output/DISK#/`
   * [] load the *.IDX file and use it to read the *.DAT files.
   * [] extract files from those new *.DAT files into the directory they're
   *    already in.
   */
  if (process_new_dat_files () != 0)
    {
      exit (1);
    }

  return 0;
}

int8_t
setup_extractor (FILE **fptr, char *filename)
{
  *fptr = fopen (filename, "rb");
  if (*fptr == NULL)
    {
      fopen_error (filename);
      return HH_FOPEN_ERROR;
    }

#ifdef _WIN32
  OP_PATH_SEPARATOR = '\\';
#else
  OP_PATH_SEPARATOR = '/';
#endif

  return 0;
}

int8_t
process_DAT_file (FILE *fptr)
{
  /*
   * First 32k (up to 0x8000) stores the `system area` of the disk.
   * Unused by HARVEST.DAT, HARVEST3.DAT, HARVEST4.DAT.
   * See: https://wiki.osdev.org/ISO_9660#System_Area
   */
  fseek (fptr, 0x8000, SEEK_SET);

  volume_descriptor vd;
  if (process_volume_descriptor_header (fptr, &vd) != 0)
    return -1;

  // Verify that this is a primary volume descriptor
  if (vd.type_code != 0x01)
    {
      puts ("Error: Unexpected volume descriptor type code.");
      printf ("\tExpected %02x, got %02x.\n", 0x01, vd.type_code);
      return -1;
    }

  if (process_volume_descriptor_data (fptr, &vd.data) != 0)
    return -1;

  size_t current_disk_name_length = strcspn (vd.data.volume_identifier, " ");
  OP_CURRENT_DISK_NAME = vd.data.volume_identifier;
  OP_CURRENT_DISK_NAME[current_disk_name_length] = '\0';

  // TODO: print the volume descriptor header/data to some file/log.

  // logical block size, in big endian form
  const uint16_t block_size_be
      = change_endianness_uint16 (vd.data.logical_block_size);

  // move to beginning of type-l path table
  fseek (fptr, block_size_be * vd.data.type_l_path_table_location, SEEK_SET);

  path_table pt;
  if (create_path_table (&pt) != 0)
    return -1;

  if (process_type_l_path_table (fptr, &pt) != 0)
    {
      destroy_path_table (&pt);
      return -1;
    }

  if (create_directories_and_extract_data_from_path_file (fptr, block_size_be,
                                                          &pt)
      != 0)
    {
      destroy_path_table (&pt);
      return -1;
    }

  // handle root directory
  fseek (fptr, block_size_be * pt.entries[0].location_of_extent, SEEK_SET);

  char *path = calloc (strlen (OP_OUTPUT_DIR) + current_disk_name_length + 2,
                       sizeof (char));
  if (path == NULL)
    {
      fprintf (stderr, CALLOC_FAILED_ERR_MSG_FMT,
               strlen (OP_OUTPUT_DIR) + current_disk_name_length + 2);
      destroy_path_table (&pt);
      return -1;
    }

  strcpy (path, OP_OUTPUT_DIR);
  strcat (path, "/");
  strcat (path, OP_CURRENT_DISK_NAME);

  if (extract_directory (fptr, block_size_be, path) != 0)
    {
      free (path);
      destroy_path_table (&pt);
      return -1;
    }

  free (path);
  destroy_path_table (&pt);
  return 0;
}

int8_t
batch_process_DAT_files ()
{
  const char *OPEN_INPUT_DIR_ERR_MSG_FMT
      = "ERROR: Error opening input directory, %s.\n";
  const uint8_t DAT_FILENAME_LEN = strlen ("HARVESTX.DAT");
  char *filename;

#ifdef _WIN32
  WIN32_FIND_DATAA file_data;
  HANDLE hFind;
  char search_path[256];
  strcpy (search_path, OP_INPUT_DIR);
  strcat (search_path, "\\*");

  hFind = FindFirstFileA (search_path, &file_data);
  if (hFind == INVALID_HANDLE_VALUE)
    {
      fprintf (stderr, OPEN_INPUT_DIR_ERR_MSG_FMT, OP_INPUT_DIR);
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
          fprintf (stderr, CALLOC_FAILED_ERR_MSG_FMT,
                   strlen (OP_INPUT_DIR) + DAT_FILENAME_LEN + 2);
          return HH_MEM_ALLOC_ERROR;
        }

      strcpy (filename, OP_INPUT_DIR);
      strcat (filename, &OP_PATH_SEPARATOR);
      strcat (filename, file_data.cFileName);

      FILE *fptr = NULL;
      if (setup_extractor (&fptr, filename) != 0)
        {
          free (filename);
          FindClose (hFind);
          return HH_FOPEN_ERROR;
        }

      if (process_DAT_file (fptr) != 0)
        {
          fclose (fptr);
          free (filename);
          FindClose (hFind);
          return -1;
        }

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
      fprintf (stderr, OPEN_INPUT_DIR_ERR_MSG_FMT, OP_INPUT_DIR);
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
          fprintf (stderr, CALLOC_FAILED_ERR_MSG_FMT,
                   strlen (OP_INPUT_DIR) + DAT_FILENAME_LEN + 2);
          return HH_MEM_ALLOC_ERROR;
        }

      strcpy (filename, OP_INPUT_DIR);
      strcat (filename, &OP_PATH_SEPARATOR);
      strcat (filename, entry->d_name);

      FILE *fptr = NULL;
      if (setup_extractor (&fptr, filename) != 0)
        {
          free (filename);
          closedir (dir);
          return HH_FOPEN_ERROR;
        }

      if (process_DAT_file (fptr) != 0)
        {
          fclose (fptr);
          free (filename);
          closedir (dir);
          return -1;
        }

      fclose (fptr);
      free (filename);
    }

  closedir (dir);
#endif
  return 0;
}

/*
 *  We DO need a more flexible/non-hardcoded solution as we don't necessarily
 *  know which DAT files the end user extracted in the first place.
 */
int8_t
process_new_dat_files (void)
{
  const uint32_t file_path_len = 3 + strlen (OP_OUTPUT_DIR) + strlen ("DISK#")
                                 + strlen ("HARVEST2.DAT");
  char *interior_dat_file_path = calloc (file_path_len, sizeof (char));
  if (interior_dat_file_path == NULL)
    {
      fprintf (stderr, CALLOC_FAILED_ERR_MSG_FMT, file_path_len);
      return HH_MEM_ALLOC_ERROR;
    }

  // for subdir in OP_OUTPUT_DIR
  strcpy (interior_dat_file_path, OP_OUTPUT_DIR);
  strcat (interior_dat_file_path, &OP_PATH_SEPARATOR);
  strcat (interior_dat_file_path, "DISK1"); // replace w/ subdir name
  strcat (interior_dat_file_path, &OP_PATH_SEPARATOR);
  // probably need to figure out a  good solution for storing these filenames
  // for the files that we are expecting.
  strcat (interior_dat_file_path, "INDEX.001");

  FILE *fptr = NULL;
  if (setup_extractor (&fptr, interior_dat_file_path) != 0)
    {
      free (interior_dat_file_path);
      return HH_FOPEN_ERROR;
    }

  index_file idx_file;
  if (create_index_file (&idx_file) != 0)
    {
      fclose (fptr);
      free (interior_dat_file_path);
      return -1;
    }

  if (process_index_file (fptr, &idx_file) != 0)
    {
      destroy_index_file (&idx_file);
      fclose (fptr);
      free (interior_dat_file_path);
      return -1;
    }

  destroy_index_file (&idx_file);
  fclose (fptr);
  free (interior_dat_file_path);
  return 0;
}
