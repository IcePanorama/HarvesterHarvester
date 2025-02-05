#include "iso_9660.h"
#include "binary_reader.h"
#include "utils.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PTABLE_STARTING_NUM_ENTRIES (10)

typedef struct PathTableEntry_s
{
  uint8_t directory_identifier_length;
  uint8_t extended_attribute_record_length;
  uint32_t extent_location;
  uint16_t parent_directory_number;
  char directory_identifier[UINT8_MAX];
} PathTableEntry_t;

/**
 *  Reads a volume descriptor type code from file.
 *  @returns zero on success, non-zero on failure.
 *  @see `enum VolumeDescriptorTypeCode_e`
 */
static int8_t
read_vd_type_code_from_file (FILE *fptr,
                             enum VolumeDescriptorTypeCode_e *output);

/**
 *  Reads primary volume descriptor data from file.
 *  @returns zero on success, non-zero on failure.
 *  @see `struct PrimaryVolumeDescriptorData_s`
 */
static int8_t
read_pvd_data_from_file (FILE *fptr,
                         struct PrimaryVolumeDescriptorData_s *pvdd);

/**
 *  Reads a directory record from file.
 *  @returns zero on success, non-zero on failure.
 *  @see `Iso9660DirectoryRecord_t`.
 */
static int8_t read_dir_rec_from_file (FILE *fptr,
                                      Iso9660DirectoryRecord_t *dr);

/**
 *  Reads a primary volume descriptor date/time data from file.
 *  @returns zero on success, non-zero on failure.
 *  @see `Iso9660PrimaryVolumeDateTime_t`
 *  @see `struct PrimaryVolumeDescriptorData_s`
 */
static int8_t
read_pvd_date_time_from_file (FILE *fptr, Iso9660PrimaryVolumeDateTime_t *dt);

/**
 *  Outputs a filesystem in a human readiable form to stdout.
 *  @see `Iso9660FileSystem_t`
 */
static void print_iso_9660_fs (Iso9660FileSystem_t *fs);

/**
 *  Outputs primary volume descriptor data in a human readiable form to stdout.
 *  @see `struct PrimaryVolumeDescriptorData_s`
 */
static void print_pvd_data (struct PrimaryVolumeDescriptorData_s *pvdd);

/**
 *  Outputs a directory entry in a human readiable form to stdout.
 *  @see `Iso9660DirectoryRecord_t`
 */
static void print_dir_rec (Iso9660DirectoryRecord_t *dr);

/**
 *  Outputs a directory entry's file flags in a human readiable form to stdout.
 *  @see `Iso9660DirectoryRecord_t`
 */
static void print_file_flags (uint8_t file_flags);

/**
 *  Outputs primary volume descriptor date/time data in a human readiable form
 *  to stdout.
 *  @see `Iso9660PrimaryVolumeDateTime_t`
 */
static void print_pvd_date_time (const char *date_time_identifier,
                                 Iso9660PrimaryVolumeDateTime_t *dt);

static int8_t extract_pvd_fs (FILE *input_fptr, Iso9660FileSystem_t *fs,
                              const char *output_dir_path);
static int8_t read_pt_from_file (FILE *fptr, PathTableEntry_t *pt);
static void print_pt (PathTableEntry_t *pt);

/**
 *  Processes all the entries in the path table that `input_fptr` currently
 *  points to, placing each entry in `pts` and updating `pt_max_size` as
 *  needed.
 *  @see `PathTableEntry_t`
 */
static int8_t process_pt_entries (FILE *input_fptr, PathTableEntry_t **pts,
                                  size_t *pt_max_size, uint32_t pt_end);

int8_t
iso_9660_create_filesystem_from_file (FILE fptr[static 1],
                                      Iso9660FileSystem_t fs[static 1])
{
  memset (fs, 0, sizeof (Iso9660FileSystem_t));
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

int8_t
read_pvd_data_from_file (FILE *fptr,
                         struct PrimaryVolumeDescriptorData_s *pvdd)
{
  if (fseek (fptr, 1, SEEK_CUR) != 0) // Unused
    goto fseek_err;

  if ((br_read_str_from_file (fptr, pvdd->system_identifier, 32) != 0)
      || (br_read_str_from_file (fptr, pvdd->volume_identifier, 32)))
    return -1;

  if (fseek (fptr, 8, SEEK_CUR) != 0) // Unused
    goto fseek_err;

  if (br_read_le_be_u32_from_file (fptr, &pvdd->volume_space_size) != 0)
    return -1;

  if (fseek (fptr, 32, SEEK_CUR) != 0) // Unused
    goto fseek_err;

  /* clang-format off */
  if ((br_read_le_be_u16_from_file (fptr, &pvdd->volume_set_size) != 0)
      || (br_read_le_be_u16_from_file (fptr, &pvdd->volume_sequence_number) != 0)
      || (br_read_le_be_u16_from_file (fptr, &pvdd->logical_block_size) != 0)
      || (br_read_le_be_u32_from_file (fptr, &pvdd->path_table_size) != 0)
      || (br_read_le_u32_from_file (fptr, &pvdd->type_l_path_table_location) != 0)
      || (br_read_le_u32_from_file (fptr, &pvdd->optional_type_l_path_table_location) != 0)
      || (br_read_be_u32_from_file (fptr, &pvdd->type_m_path_table_location) != 0)
      || (br_read_be_u32_from_file (fptr, &pvdd->optional_type_m_path_table_location) != 0)
      || (read_dir_rec_from_file (fptr, &pvdd->root_directory_entry) != 0)
      || (br_read_str_from_file (fptr, pvdd->volume_set_identifier, 128) != 0)
      || (br_read_str_from_file (fptr, pvdd->publisher_identifier, 128) != 0)
      || (br_read_str_from_file (fptr, pvdd->data_preparer_identifier, 128) != 0)
      || (br_read_str_from_file (fptr, pvdd->application_identifier, 128) != 0)
      || (br_read_str_from_file (fptr, pvdd->copyright_file_identifier, 37) != 0)
      || (br_read_str_from_file (fptr, pvdd->abstract_file_identifier, 37) != 0)
      || (br_read_str_from_file (fptr, pvdd->bibliographic_file_identifier, 37) != 0)
      || (read_pvd_date_time_from_file (fptr, &pvdd->volume_creation_date_time) != 0)
      || (read_pvd_date_time_from_file (fptr, &pvdd->volume_modification_date_time) != 0)
      || (read_pvd_date_time_from_file (fptr, &pvdd->volume_expiration_date_time) != 0)
      || (read_pvd_date_time_from_file (fptr, &pvdd->volume_effective_date_time) != 0)
      || (br_read_u8_from_file (fptr, &pvdd->file_structure_version) != 0)
      || (br_read_u8_array_from_file(fptr, pvdd->application_used_data, 512) != 0))
    return -1;
  /* clang-format on */

  return 0;
fseek_err:
  fprintf (stderr, "ERROR: failed to seek past unused byte(s) in primary "
                   "volume descriptor data.\n");
  return -1;
}

int8_t
read_dir_rec_from_file (FILE *fptr, Iso9660DirectoryRecord_t *dr)
{
  if ((br_read_u8_from_file (fptr, &dr->dir_rec_length) != 0)
      || (br_read_u8_from_file (fptr, &dr->extended_attrib_rec_length) != 0)
      || (br_read_le_be_u32_from_file (fptr, &dr->extent_location) != 0)
      || (br_read_le_be_u32_from_file (fptr, &dr->extent_size) != 0)
      || (br_read_u8_from_file (fptr, &dr->recording_date_time.year) != 0)
      || (br_read_u8_from_file (fptr, &dr->recording_date_time.month) != 0)
      || (br_read_u8_from_file (fptr, &dr->recording_date_time.day) != 0)
      || (br_read_u8_from_file (fptr, &dr->recording_date_time.hour) != 0)
      || (br_read_u8_from_file (fptr, &dr->recording_date_time.minute) != 0)
      || (br_read_u8_from_file (fptr, &dr->recording_date_time.second) != 0)
      || (br_read_u8_from_file (fptr, &dr->recording_date_time.timezone) != 0)
      || (br_read_u8_from_file (fptr, &dr->file_flags) != 0)
      || (br_read_u8_from_file (fptr, &dr->file_unit_size) != 0)
      || (br_read_u8_from_file (fptr, &dr->interleave_gap_size) != 0)
      || (br_read_le_be_u16_from_file (fptr, &dr->volume_sequence_number) != 0)
      || (br_read_u8_from_file (fptr, &dr->file_identifier_length) != 0)
      || (br_read_str_from_file (fptr, dr->file_identifier,
                                 dr->file_identifier_length)
          != 0))
    {
      return -1;
    }

  if ((dr->file_identifier_length % 2) == 0)
    {
      if (fseek (fptr, 1, SEEK_CUR) != 0)
        {
          fprintf (
              stderr,
              "ERROR: failed to seek past padding after directory record.\n");
          return -1;
        }
    }

  return 0;
}

void
print_iso_9660_fs (Iso9660FileSystem_t *fs)
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
read_pvd_date_time_from_file (FILE *fptr, Iso9660PrimaryVolumeDateTime_t *dt)
{
  if ((br_read_str_from_file (fptr, dt->year, 4) != 0)
      || (br_read_str_from_file (fptr, dt->month, 2) != 0)
      || (br_read_str_from_file (fptr, dt->day, 2) != 0)
      || (br_read_str_from_file (fptr, dt->hour, 2) != 0)
      || (br_read_str_from_file (fptr, dt->minute, 2) != 0)
      || (br_read_str_from_file (fptr, dt->second, 2) != 0)
      || (br_read_str_from_file (fptr, dt->hundredths_of_a_second, 2) != 0)
      || (br_read_u8_from_file (fptr, &dt->timezone) != 0))
    {
      return -1;
    }

  return 0;
}

void
print_pvd_data (struct PrimaryVolumeDescriptorData_s *pvdd)
{
  printf ("- System directory_identifier: %.*s\n", 32,
          pvdd->system_identifier);
  printf ("- Volume directory_identifier: %.*s\n", 32,
          pvdd->volume_identifier);
  printf ("- Volume space size: %d\n", pvdd->volume_space_size);
  printf ("- Volume set size: %d\n", pvdd->volume_set_size);
  printf ("- Volume sequence number: %d\n", pvdd->volume_sequence_number);
  printf ("- Logical block size: %d\n", pvdd->logical_block_size);

  printf ("- Path table size: %d\n", pvdd->path_table_size);
  printf ("- Location of type-l path table: 0x%08X\n",
          pvdd->type_l_path_table_location);
  printf ("- Location of optional type-l path table: 0x%08X\n",
          pvdd->optional_type_l_path_table_location);
  printf ("- Location of type-m path table: 0x%08X\n",
          pvdd->type_m_path_table_location);
  printf ("- Location of optional type-m path table: 0x%08X\n",
          pvdd->optional_type_m_path_table_location);

  puts ("- Root directory entry:");
  print_dir_rec (&pvdd->root_directory_entry);

  printf ("- Volume set directory_identifier: %.*s\n", 128,
          pvdd->volume_set_identifier);
  printf ("- Publisher directory_identifier: %.*s\n", 128,
          pvdd->publisher_identifier);
  printf ("- Data preparer directory_identifier: %.*s\n", 128,
          pvdd->data_preparer_identifier);
  printf ("- Application directory_identifier: %.*s\n", 128,
          pvdd->application_identifier);
  printf ("- Copyright file directory_identifier: %.*s\n", 37,
          pvdd->copyright_file_identifier);
  printf ("- Abstract file directory_identifier: %.*s\n", 37,
          pvdd->abstract_file_identifier);
  printf ("- Bibliographic file directory_identifier: %.*s\n", 37,
          pvdd->bibliographic_file_identifier);

  print_pvd_date_time ("- Volume creation date time",
                       &pvdd->volume_creation_date_time);
  print_pvd_date_time ("- Volume modification date time",
                       &pvdd->volume_modification_date_time);
  print_pvd_date_time ("- Volume expiration date time",
                       &pvdd->volume_expiration_date_time);
  print_pvd_date_time ("- Volume effective date time",
                       &pvdd->volume_effective_date_time);

  printf ("- File structure version: %d\n", pvdd->file_structure_version);
}

void
print_dir_rec (Iso9660DirectoryRecord_t *dr)
{
  printf ("-- Directory record length: %d\n", dr->dir_rec_length);
  printf ("-- Extended attribute length: %d\n",
          dr->extended_attrib_rec_length);
  printf ("-- Location of extent: 0x%08X\n", dr->extent_location);
  printf ("-- Size of extent: %d\n", dr->extent_size);

  printf ("-- Recording date/time: %04d-%02d-%02d %02d:%02d:%02d (GMT%+02d)\n",
          1900 + dr->recording_date_time.year, dr->recording_date_time.month,
          dr->recording_date_time.day, dr->recording_date_time.hour,
          dr->recording_date_time.minute, dr->recording_date_time.second,
          (-48 + dr->recording_date_time.timezone) >> 2);

  puts ("-- File flags:");
  print_file_flags (dr->file_flags);

  printf ("-- File unit size: %d\n", dr->file_unit_size);
  printf ("-- Interleave gap size: %d\n", dr->interleave_gap_size);
  printf ("-- Volume sequence number: %d\n", dr->volume_sequence_number);
  printf ("-- File directory_identifier length: %d\n",
          dr->file_identifier_length);
  printf ("-- File directory_identifier: %.*s\n", dr->file_identifier_length,
          dr->file_identifier);
}

void
print_file_flags (uint8_t file_flags)
{
  printf ("--- Hidden? %s\n", (file_flags & 1) ? "true" : "false");
  printf ("--- Directory? %s\n", (file_flags & 2) ? "true" : "false");
  printf ("--- Associated file? %s\n", (file_flags & 4) ? "true" : "false");
  printf ("--- Format info in extended attrib. record? %s\n",
          (file_flags & 8) ? "true" : "false");
  printf ("--- Owner/group perms in extended attrib. record? %s\n",
          (file_flags & 16) ? "true" : "false");
  printf ("--- Not final directory record? %s\n",
          (file_flags & 128) ? "true" : "false");
}

void
print_pvd_date_time (const char *date_time_identifier,
                     Iso9660PrimaryVolumeDateTime_t *dt)
{
  printf ("%s: %.4s-%.2s-%.2s %.2s:%.2s:%.2s.%.2s (GMT%+02d)\n",
          date_time_identifier, dt->year, dt->month, dt->day, dt->hour,
          dt->minute, dt->second, dt->hundredths_of_a_second,
          (-48 + dt->timezone) >> 2);
}

int8_t
iso_9660_extract_filesystem (FILE input_fptr[static 1],
                             Iso9660FileSystem_t fs[static 1],
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
extract_pvd_fs (FILE *input_fptr, Iso9660FileSystem_t *fs,
                const char *output_dir_path)
{
  // tmp
  printf ("tmp, delete me! %s.\n", output_dir_path);

  /** Logical block size. Using this var for readability's sake. */
  uint16_t block_size
      = fs->volume_desc_data.primary_vol_desc.logical_block_size;
  uint64_t pt_start
      = fs->volume_desc_data.primary_vol_desc.type_l_path_table_location
        * block_size;
  uint64_t pt_end
      = pt_start + fs->volume_desc_data.primary_vol_desc.path_table_size;

  if (fseek (input_fptr, pt_start, SEEK_SET) != 0)
    {
      fprintf (
          stderr,
          "ERROR: failed to seek to root directory path table (0x%08lX).\n",
          pt_start);
      return -1;
    }

  size_t max_num_ptable_entries = (PTABLE_STARTING_NUM_ENTRIES);
  PathTableEntry_t *root_pt_entries
      = calloc (max_num_ptable_entries, sizeof (PathTableEntry_t));
  if (root_pt_entries == NULL)
    {
      fprintf (stderr,
               "ERROR: Failed to alloc path table array of size, %ld\n",
               max_num_ptable_entries);
      return -1;
    }

  if (process_pt_entries (input_fptr, &root_pt_entries,
                          &max_num_ptable_entries, pt_end)
      != 0)
    {
      goto clean_up;
    }

  for (size_t i = max_num_ptable_entries - 1; i > 0; i--)
    {
      PathTableEntry_t *curr = &root_pt_entries[i];
      size_t path_len = curr->directory_identifier_length + 1;
      char *path = calloc (path_len, sizeof (char));
      if (path == NULL)
        {
          fprintf (stderr, "ERROR: Unable to alloc path string of size, %ld\n",
                   path_len);
          goto clean_up;
        }

      strncpy (path, curr->directory_identifier,
               curr->directory_identifier_length);
      path[path_len - 1] = '\0';

      do
        {
          // `parent_directory_number` is 1-indexed.
          curr = &root_pt_entries[curr->parent_directory_number - 1];

          if (u_prepend_path_str (&path, curr->directory_identifier,
                                  curr->directory_identifier_length)
              != 0)
            {
              free (path);
              goto clean_up;
            }
          printf ("Path: %s\n", path);
        }
      while (curr->parent_directory_number > 1);

      free (path);
    }

  free (root_pt_entries);
  return 0;
clean_up:
  free (root_pt_entries);
  return -1;
}

int8_t
read_pt_from_file (FILE *fptr, PathTableEntry_t *pt)
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
print_pt (PathTableEntry_t *pt)
{
  printf ("%.*s - Ext. attrib. len: %d, Loc: %d, Parent: %d\n",
          pt->directory_identifier_length, pt->directory_identifier,
          pt->extended_attribute_record_length, pt->extent_location,
          pt->parent_directory_number);
}

int8_t
process_pt_entries (FILE *input_fptr, PathTableEntry_t **pts,
                    size_t *pt_max_size, uint32_t pt_end)
{
  size_t i = 0;
  uint32_t pos = 0;
  do
    {
      if (i >= (*pt_max_size))
        {
          (*pt_max_size) *= 2;
          PathTableEntry_t *tmp
              = realloc ((*pts), sizeof (PathTableEntry_t) * (*pt_max_size));
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
