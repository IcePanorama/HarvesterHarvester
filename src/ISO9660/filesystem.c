#include "binary_reader.h"
#include "utils.h"

#include "ISO9660/filesystem.h"
#include "ISO9660/path_table_entry.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PTABLE_STARTING_NUM_ENTRIES (10)
#define LIST_DIR_RECORD_STARTING_NUM_ENTRIES ((PTABLE_STARTING_NUM_ENTRIES))

/**
 *  Reads a volume descriptor type code from file.
 *  @returns zero on success, non-zero on failure.
 *  @see `enum VolumeDescriptorTypeCode_e`
 */
static int8_t
read_vd_type_code_from_file (FILE *fptr,
                             enum VolumeDescriptorTypeCode_e *output);

/**
 *  Outputs a filesystem in a human readiable form to stdout.
 *  @see `ISO9660FileSystem_t`
 */
static void print_iso_9660_fs (ISO9660FileSystem_t *fs);

static int8_t extract_pvd_fs (FILE *input_fptr, ISO9660FileSystem_t *fs,
                              const char *output_dir_path);
static int8_t read_pt_from_file (FILE *fptr, ISO9660PathTableEntry_t *pt);
static void print_pt (ISO9660PathTableEntry_t *pt);

/**
 *  Processes all the entries in the path table that `input_fptr` currently
 *  points to, placing each entry in `pts` and updating `pt_max_size` as
 *  needed.
 *  @see `ISO9660PathTableEntry_t`
 */
static int8_t process_pt_entries (FILE *input_fptr,
                                  ISO9660PathTableEntry_t **pts,
                                  size_t *pt_max_size, uint32_t pt_end);
static int alloc_pt_entries_array (ISO9660PathTableEntry_t **pts, size_t size);

/**
 *  Calculates the length of a primary volume descriptor's volume identifier.
 *  This string isn't NULL-terminated, therefore this function looks for the
 *  first space character.
 *  @returns the position of the first space character in `volume_identifier`
 */
static uint_least8_t calculate_pvd_vol_id_len (const char *volume_identifier);

/**
 *  Creates a path string for every entry in a given path table.
 *  @returns zero on success, non-zero on failure.
 */
static int build_paths_from_pt_list (ISO9660PathTableEntry_t *pt_list,
                                     char **path_list, size_t list_len,
                                     const char *pvd_vol_id,
                                     const char *output_dir);

static int populate_directory_record_list (FILE *input_fptr, uint16_t lbs,
                                           ISO9660PathTableEntry_t *pt_list,
                                           size_t pt_list_len,
                                           ISO9660DirectoryRecord_t **dr_list,
                                           size_t *dr_list_lens);

/*
 *  When processing a list of directory records: "even if a directory spans
 *  multiple sectors, the directory entries are not permitted to cross the
 * sector boundary ... where there is not enough space to record an entire
 * directory entry at the end of a sector, that sector is zero-padded and the
 * next consecutive sector is used."
 *  @param  fptr  the file to seek through.
 *  @param  lbs   the logical block size of the given file's filesystem.
 *  @see: https://wiki.osdev.org/ISO_9660#Directories
 */
static int handle_sector_boundary_padding (FILE *fptr, uint16_t lbs);

int8_t
iso_9660_create_filesystem_from_file (FILE fptr[static 1],
                                      ISO9660FileSystem_t fs[static 1])
{
  memset (fs, 0, sizeof (ISO9660FileSystem_t));
  if (fseek (fptr, 0x8000, SEEK_SET) != 0)
    {
      fprintf (stderr, "ERROR: failed to seek past system area (32KiB).\n");
      return -1;
    }

  if ((read_vd_type_code_from_file (fptr, &fs->volume_desc_type_code) != 0)
      || (br_read_str_from_file (fptr, fs->volume_identifier, 5) != 0)
      || (br_read_u8_from_file (fptr, &fs->volume_desc_version_num)))
    return -1;

  switch (fs->volume_desc_type_code)
    {
    case VDTC_PRIMARY_VOLUME:
      if (read_pvd_data_from_file (fptr,
                                   &fs->volume_desc_data.primary_vol_desc)
          != 0)
        return -1;
      break;
    case VDTC_BOOT_RECORD:
    case VDTC_SUPPLEMENTARY_VOL:
    case VDTC_VOL_PARTITION:
    case VDTC_VOL_DESC_SET_TERMINATOR:
      fprintf (stderr, "ERROR: No implemented support for type code, %02X.\n",
               fs->volume_desc_type_code);
      return -1;
    }

  print_iso_9660_fs (fs);

  return 0;
}

int8_t
read_vd_type_code_from_file (FILE *fptr,
                             enum VolumeDescriptorTypeCode_e *output)
{
  uint8_t byte;
  if (br_read_u8_from_file (fptr, &byte) != 0)
    return -1;

  if ((byte > VDTC_VOL_PARTITION) && (byte != VDTC_VOL_DESC_SET_TERMINATOR))
    {
      fprintf (stderr, "ERROR: unknown volume descriptor type code: %02X.\n",
               byte);
      return -1;
    }

  *output = (enum VolumeDescriptorTypeCode_e) (byte);
  return 0;
}

void
print_iso_9660_fs (ISO9660FileSystem_t *fs)
{
  printf ("Volume descriptor type code: %02X\n", fs->volume_desc_type_code);
  printf ("Volume identifier: %.*s\n", 5, fs->volume_identifier);
  printf ("Volume descriptor version: %02X\n", fs->volume_desc_version_num);

  switch (fs->volume_desc_type_code)
    {
    case VDTC_PRIMARY_VOLUME:
      puts ("Primary volume data: ");
      print_pvd_data (&fs->volume_desc_data.primary_vol_desc);
      break;
    default:
      break;
    }
}

int8_t
iso_9660_extract_filesystem (FILE input_fptr[static 1],
                             ISO9660FileSystem_t fs[static 1],
                             const char output_dir_path[static 1])
{
  switch (fs->volume_desc_type_code)
    {
    case VDTC_PRIMARY_VOLUME:
      if (extract_pvd_fs (input_fptr, fs, output_dir_path) != 0)
        return -1;
      break;
    default:
      fprintf (stderr,
               "ERROR: No implemented support for extracting filesystems of "
               "type code, %02X.\n",
               fs->volume_desc_type_code);
      return -1;
    }

  return 0;
}

int8_t
extract_pvd_fs (FILE *input_fptr, ISO9660FileSystem_t *fs,
                const char *output_dir_path)
{
  size_t max_num_ptable_entries = (PTABLE_STARTING_NUM_ENTRIES);
  ISO9660PathTableEntry_t *root_pt_entries;
  if (alloc_pt_entries_array (&root_pt_entries, max_num_ptable_entries) != 0)
    return -1;

  // Defining the following variables just for readability's sake
  ISO9660PrimaryVolumeDescriptorData_t pvd
      = fs->volume_desc_data.primary_vol_desc;
  /** Logical block size. */
  uint16_t block_size = pvd.logical_block_size;
  /** Path table start */
  uint64_t pt_start = pvd.type_l_path_table_location * block_size;
  uint64_t pt_end = pt_start + pvd.path_table_size;

  if (fseek (input_fptr, pt_start, SEEK_SET) != 0)
    {
      fprintf (
          stderr,
          "ERROR: failed to seek to root directory path table (0x%08lX).\n",
          pt_start);
      goto clean_up;
    }

  if (process_pt_entries (input_fptr, &root_pt_entries,
                          &max_num_ptable_entries, pt_end)
      != 0)
    goto clean_up;

  char **path_list = calloc (max_num_ptable_entries, sizeof (char *));
  if (path_list == NULL)
    {
      fprintf (stderr,
               "ERROR: Failed to allocate memory for list of path strings.\n");
      goto clean_up;
    }

  if (build_paths_from_pt_list (root_pt_entries, path_list,
                                max_num_ptable_entries, pvd.volume_identifier,
                                output_dir_path)
      != 0)
    goto clean_up2;

  ISO9660DirectoryRecord_t **dr_list;
  dr_list
      = calloc (max_num_ptable_entries, sizeof (ISO9660DirectoryRecord_t *));
  if (dr_list == NULL)
    {
      fprintf (
          stderr,
          "ERROR: Failed to allocate memory for directory record list.\n");
      goto clean_up3;
    }

  size_t *dr_list_lens = calloc (max_num_ptable_entries, sizeof (size_t));
  if (dr_list_lens == NULL)
    goto clean_up4;

  if (populate_directory_record_list (input_fptr, block_size, root_pt_entries,
                                      max_num_ptable_entries, dr_list,
                                      dr_list_lens)
      != 0)
    goto clean_up5;

  free (dr_list_lens);
  u_free_list_of_elements ((void **)dr_list, max_num_ptable_entries);
  free (dr_list);
  u_free_list_of_elements ((void **)path_list, max_num_ptable_entries);
  free (path_list);
  free (root_pt_entries);
  return 0;

  // FIXME: refactor all this lmao
clean_up5:
  free (dr_list_lens);
clean_up4:
  free (dr_list);
clean_up3:
  u_free_list_of_elements ((void **)path_list, max_num_ptable_entries);
clean_up2:
  free (path_list);
clean_up:
  free (root_pt_entries);
  return -1;
}

int8_t
read_pt_from_file (FILE *fptr, ISO9660PathTableEntry_t *pt)
{
  if ((br_read_u8_from_file (fptr, &pt->directory_identifier_length) != 0)
      || (br_read_u8_from_file (fptr, &pt->extended_attribute_record_length)
          != 0)
      || (br_read_le_u32_from_file (fptr, &pt->extent_location) != 0)
      || (br_read_le_u16_from_file (fptr, &pt->parent_directory_number) != 0)
      || (br_read_str_from_file (fptr, pt->directory_identifier,
                                 pt->directory_identifier_length)
          != 0))
    {
      return -1;
    }

  if ((pt->directory_identifier_length % 2) != 0)
    {
      if (fseek (fptr, 1, SEEK_CUR) != 0)
        {
          fprintf (stderr,
                   "ERROR: failed to seek past padding after path table.\n");
          return -1;
        }
    }

  return 0;
}

void
print_pt (ISO9660PathTableEntry_t *pt)
{
  printf ("%.*s - Ext. attrib. len: %d, Loc: %d, Parent: %d\n",
          pt->directory_identifier_length, pt->directory_identifier,
          pt->extended_attribute_record_length, pt->extent_location,
          pt->parent_directory_number);
}

int8_t
process_pt_entries (FILE *input_fptr, ISO9660PathTableEntry_t **pts,
                    size_t *pt_max_size, uint32_t pt_end)
{
  size_t i = 0;
  uint32_t pos = 0;
  do
    {
      if (i >= (*pt_max_size))
        {
          (*pt_max_size) *= 2;
          ISO9660PathTableEntry_t *tmp = realloc (
              (*pts), sizeof (ISO9660PathTableEntry_t) * (*pt_max_size));
          if (tmp == NULL)
            {
              fprintf (stderr,
                       "ERROR: Failed to grow path table array to size, %ld\n",
                       (*pt_max_size));
              return -1;
            }
          (*pts) = tmp;
        }

      if (read_pt_from_file (input_fptr, &(*pts)[i]) != 0)
        {
          return -1;
        }
      print_pt (&(*pts)[i]);
      i++;

      pos = (uint32_t)(ftell (input_fptr));
    }
  while (pos != pt_end);

  (*pt_max_size) = i;

  return 0;
}

int
alloc_pt_entries_array (ISO9660PathTableEntry_t **pts, size_t size)
{
  (*pts) = calloc (size, sizeof (ISO9660PathTableEntry_t));
  if ((*pts) == NULL)
    {
      fprintf (stderr,
               "ERROR: Failed to alloc path table array of size, %ld\n", size);
      return -1;
    }

  return 0;
}

int
build_paths_from_pt_list (ISO9660PathTableEntry_t *pt_list, char **path_list,
                          size_t list_len, const char *pvd_vol_id,
                          const char *output_dir)
{
  const uint_least8_t pvd_vol_id_len = calculate_pvd_vol_id_len (pvd_vol_id);

  for (size_t i = list_len - 1; i > 0; i--)
    {
      ISO9660PathTableEntry_t *curr = &pt_list[i];
      size_t path_len = curr->directory_identifier_length + 1;

      char *path = calloc (path_len, sizeof (char));
      if (path == NULL)
        {
          fprintf (stderr, "ERROR: Unable to alloc path string of size, %ld\n",
                   path_len);
          u_free_partial_list_elements ((void **)path_list, i + 1, list_len);
          return -1;
        }

      strncpy (path, curr->directory_identifier,
               curr->directory_identifier_length);
      path[path_len - 1] = '\0';

      // Recursively build path string by following `curr`'s parent dir.
      ISO9660PathTableEntry_t *tmp = curr;
      do
        {
          // `parent_directory_number` is 1-indexed.
          tmp = &pt_list[tmp->parent_directory_number - 1];

          /*
           *  `directory_identifier` is NOT NULL-terminated, hence the need for
           *  passing its length as well.
           */
          if (u_prepend_path_str (&path, tmp->directory_identifier,
                                  tmp->directory_identifier_length)
              != 0)
            {
              u_free_partial_list_elements ((void **)path_list, i + 1,
                                            list_len);
              free (path);
              return -1;
            }
        }
      while (tmp->parent_directory_number > 1);

      u_prepend_path_str (&path, pvd_vol_id, pvd_vol_id_len);
      u_prepend_path_str (&path, output_dir, strlen (output_dir));
      path_list[i] = path;
      printf ("Path: %s\n", path_list[i]);
    }

  // handle root dir, 2 for path separator & NULL-terminator
  char *path
      = calloc (pvd_vol_id_len + strlen (output_dir) + 2, sizeof (char));
  if (path == NULL)
    {
      u_free_partial_list_elements ((void **)path_list, 1, list_len);
      return -1;
    }

  strncpy (path, pvd_vol_id, pvd_vol_id_len);
  u_prepend_path_str (&path, output_dir, strlen (output_dir));
  path_list[0] = path;
  printf ("Path: %s\n", path_list[0]);

  return 0;
}

uint_least8_t
calculate_pvd_vol_id_len (const char *volume_identifier)
{
  size_t i;
  for (i = 0; i < 32; i++)
    {
      if (volume_identifier[i] == ' ')
        break;
    }

  return i;
}

int
populate_directory_record_list (FILE *input_fptr, uint16_t lbs,
                                ISO9660PathTableEntry_t *pt_list,
                                size_t pt_list_len,
                                ISO9660DirectoryRecord_t **dr_list,
                                size_t *dr_list_lens)
{
  size_t i, j;

  for (i = 0; i < pt_list_len; i++)
    {
      dr_list_lens[i] = (LIST_DIR_RECORD_STARTING_NUM_ENTRIES);
      dr_list[i] = calloc (dr_list_lens[i], sizeof (ISO9660DirectoryRecord_t));
      if (dr_list[i] == NULL)
        {
          fprintf (
              stderr,
              "ERROR: Failed to allocate memory for directory record list.");
          goto clean_up;
        }

      if (fseek (input_fptr, pt_list[i].extent_location * lbs, SEEK_SET) != 0)
        {
          fprintf (
              stderr,
              "ERROR: Failed to seek to directory record location (0x%08X).\n",
              pt_list[i].extent_location * lbs);
          goto clean_up;
        }

      j = 0;
      while (1)
        {
          if (handle_sector_boundary_padding (input_fptr, lbs) != 0)
            goto clean_up;

          ISO9660DirectoryRecord_t dir;
          if (read_dir_rec_from_file (input_fptr, &dir) != 0)
            goto clean_up;

          if (dir.file_identifier_length == 0)
            break;

          if (j >= dr_list_lens[i])
            {
              dr_list_lens[i] *= 2;
              ISO9660DirectoryRecord_t *temp = realloc (
                  dr_list[i],
                  dr_list_lens[i] * sizeof (ISO9660DirectoryRecord_t));
              if (temp == NULL)
                {
                  fprintf (stderr, "ERROR: Failed to reallocate memory for "
                                   "directory record.");
                  goto realloc_failure;
                }
              dr_list[i] = temp;
            }

          memcpy ((void *)&dr_list[i][j], &dir,
                  sizeof (ISO9660DirectoryRecord_t));
          j++;
        }

      dr_list_lens[i] = j;
    }

  return 0;
realloc_failure:
  u_free_partial_list_elements ((void **)&dr_list[i], 0, j);
clean_up:
  u_free_partial_list_elements ((void **)dr_list, 0, i);
  return -1;
}

int
handle_sector_boundary_padding (FILE *fptr, uint16_t lbs)
{
  size_t curr_sector = (size_t)(ftell (fptr) / lbs);
  size_t next_sector
      = (size_t)((ftell (fptr) + sizeof (ISO9660DirectoryRecord_t)) / lbs);
  if (next_sector != curr_sector) // Entry crosses sector boundary
    {
      if (fseek (fptr, (size_t)((ftell (fptr) + (lbs - 1)) / lbs), SEEK_SET)
          != 0)
        {
          fprintf (stderr, "ERROR: Failed to seek across sector boundary.\n");
          return -1;
        }
    }

  return 0;
}
