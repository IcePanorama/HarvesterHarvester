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
#include "data_reader.h"
#include "errors.h"
#include "extractor.h"
#include "index_file.h"
#include "log.h"
#include "options.h"
#include "path_table.h"
#include "utils.h"
#include "volume_descriptor.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

  if (!OP_SKIP_INT_DAT_PROCESSING && process_internal_dat_files () != 0)
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
      hh_log (HH_LOG_ERROR,
              "Unexpected volume descriptor type code. Expected 0x%02X, got "
              "0x%02X.",
              0x01, vd.type_code);
      return -1;
    }

  if (process_volume_descriptor_data (fptr, &vd.data) != 0)
    return -1;

  size_t current_disk_name_length = strcspn (vd.data.volume_identifier, " ");
  CURRENT_DISK_NAME = vd.data.volume_identifier;
  CURRENT_DISK_NAME[current_disk_name_length] = '\0';

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
      destroy_path_table (&pt);
      return handle_calloc_error (strlen (OP_OUTPUT_DIR)
                                  + current_disk_name_length + 2);
    }

  strcpy (path, OP_OUTPUT_DIR);
  strcat (path, &OP_PATH_SEPARATOR);
  strcat (path, CURRENT_DISK_NAME);

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
      = "[HarvesterHarvester]ERROR: Error opening input directory, %s.";
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
      hh_log (HH_LOG_ERROR, OPEN_INPUT_DIR_ERR_MSG_FMT, OP_INPUT_DIR);
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
          return handle_calloc_error (strlen (OP_INPUT_DIR) + DAT_FILENAME_LEN
                                      + 2);
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
      hh_log (HH_LOG_ERROR, OPEN_INPUT_DIR_ERR_MSG_FMT, OP_INPUT_DIR);
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
          return handle_calloc_error (strlen (OP_INPUT_DIR) + DAT_FILENAME_LEN
                                      + 2);
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

int8_t
process_internal_dat_files (void)
{
  const char *interal_paths = "internal-dat-file-paths.txt";
  FILE *table = fopen (interal_paths, "rb");
  if (table == NULL)
    {
      fopen_error ((char *)interal_paths);
      return HH_FOPEN_ERROR;
    }

  while (!peek_eof (table))
    {
      // TODO: convert as many malloc'd strings to char arrays as possible.
      char index_file_path[256] = { 0 };
      build_path_string_from_file (table, index_file_path);

      FILE *fptr = NULL;
      if (setup_extractor (&fptr, index_file_path) != 0)
        {
          hh_log (HH_LOG_WARNING, "Skipping...");

          // Skip next line
          build_path_string_from_file (table, index_file_path);

          continue;
        }

      index_file idx_file;
      if (create_index_file (&idx_file) != 0)
        {
          fclose (fptr);
          fclose (table);
          return -1;
        }

      hh_log (HH_LOG_INFO, "Processing index file: %s", index_file_path);
      if (process_index_file (fptr, &idx_file) != 0)
        {
          destroy_index_file (&idx_file);
          fclose (table);
          fclose (fptr);
          return -1;
        }
      fclose (fptr);

      char dat_file_path[256] = { 0 };
      build_path_string_from_file (table, dat_file_path);

      if (extract_index_file (&idx_file, index_file_path, dat_file_path) != 0)
        {
          destroy_index_file (&idx_file);
          return -1;
        }

      destroy_index_file (&idx_file);
    }

  fclose (table);
  return 0;
}
