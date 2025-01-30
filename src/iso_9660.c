#include "iso_9660.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct PathTable_s
{
  uint8_t directory_identifier_length;
  uint8_t extended_attribute_record_length;
  uint32_t extent_location;
  uint16_t parent_directory_number;
  char directory_identifier[UINT8_MAX + 3];
} PathTable_t;

/**
 *  Reads a volume descriptor type code from file.
 *  @returns zero on success, non-zero on failure.
 *  @see `enum VolumeDescriptorTypeCode_e`
 */
static int8_t
read_vd_type_code_from_file (FILE *fptr,
                             enum VolumeDescriptorTypeCode_e *output);
/** @returns zero on success, non-zero on failure. */
static int8_t read_uint8_from_file (FILE *fptr, uint8_t *output);
/** @returns zero on success, non-zero on failure. */
static int8_t read_str_from_file (FILE *fptr, char *output, size_t length);
/**
 *  Reads primary volume descriptor data from file.
 *  @returns zero on success, non-zero on failure.
 *  @see `struct PrimaryVolumeDescriptorData_s`
 */
static int8_t
read_pvd_data_from_file (FILE *fptr,
                         struct PrimaryVolumeDescriptorData_s *pvdd);
/**
 *  Reads a little-endian followed by big-endian encoded unsigned 32-bit
 *  integer. Double checks that these values match because, why not?
 *  @returns zero on success, non-zero on failure.
 *  @see https://wiki.osdev.org/ISO_9660#Numerical_formats
 */
static int8_t read_le_be_uint32_from_file (FILE *fptr, uint32_t *output);
/**
 *  Reads a little endian 32-bit integer from file.
 *  @returns zero on success, non-zero on failure.
 */
static int8_t read_le_uint32_from_file (FILE *fptr, uint32_t *output);
/**
 *  Reads a big endian 32-bit integer from file.
 *  @returns zero on success, non-zero on failure.
 */
static int8_t read_be_uint32_from_file (FILE *fptr, uint32_t *output);
/**
 *  Reads a little-endian followed by big-endian encoded unsigned 16-bit
 *  integer. Double checks that these values match because, why not?
 *  @returns zero on success, non-zero on failure.
 *  @see https://wiki.osdev.org/ISO_9660#Numerical_formats
 */
static int8_t read_le_be_uint16_from_file (FILE *fptr, uint16_t *output);
/**
 *  Reads a little endian 16-bit integer from file.
 *  @returns zero on success, non-zero on failure.
 */
static int8_t read_le_uint16_from_file (FILE *fptr, uint16_t *output);
/**
 *  Reads a big endian 16-bit integer from file.
 *  @returns zero on success, non-zero on failure.
 */
static int8_t read_be_uint16_from_file (FILE *fptr, uint16_t *output);
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
/** @returns zero on success, non-zero on failure. */
static int8_t read_uint8_array_from_file (FILE *fptr, uint8_t *output,
                                          size_t length);
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
static int8_t read_pt_from_file (FILE *fptr, PathTable_t *pt);
static void print_pt (PathTable_t *pt);

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
      || (read_str_from_file (fptr, fs->volume_identifier, 5) != 0)
      || (read_uint8_from_file (fptr, &fs->volume_desc_version_num)))
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
  if (read_uint8_from_file (fptr, &byte) != 0)
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
read_uint8_from_file (FILE *fptr, uint8_t *output)
{
  size_t bytes_read = fread (output, sizeof (uint8_t), 1, fptr);
  if (bytes_read != sizeof (uint8_t))
    {
      fprintf (stderr, "ERROR: Error reading uint8 from file.\n");
      return -1;
    }

  return 0;
}

int8_t
read_str_from_file (FILE *fptr, char *output, size_t length)
{
  size_t bytes_read = fread (output, sizeof (char), length, fptr);
  if (bytes_read != (sizeof (char) * length))
    {
      fprintf (stderr, "ERROR: Error reading string from file.\n");
      return -1;
    }

  return 0;
}

int8_t
read_pvd_data_from_file (FILE *fptr,
                         struct PrimaryVolumeDescriptorData_s *pvdd)
{
  if (fseek (fptr, 1, SEEK_CUR) != 0) // Unused
    goto fseek_err;

  if ((read_str_from_file (fptr, pvdd->system_identifier, 32) != 0)
      || (read_str_from_file (fptr, pvdd->volume_identifier, 32)))
    return -1;

  if (fseek (fptr, 8, SEEK_CUR) != 0) // Unused
    goto fseek_err;

  if (read_le_be_uint32_from_file (fptr, &pvdd->volume_space_size) != 0)
    return -1;

  if (fseek (fptr, 32, SEEK_CUR) != 0) // Unused
    goto fseek_err;

  /* clang-format off */
  if ((read_le_be_uint16_from_file (fptr, &pvdd->volume_set_size) != 0)
      || (read_le_be_uint16_from_file (fptr, &pvdd->volume_sequence_number) != 0)
      || (read_le_be_uint16_from_file (fptr, &pvdd->logical_block_size) != 0)
      || (read_le_be_uint32_from_file (fptr, &pvdd->path_table_size) != 0)
      || (read_le_uint32_from_file (fptr, &pvdd->type_l_path_table_location) != 0)
      || (read_le_uint32_from_file (fptr, &pvdd->optional_type_l_path_table_location) != 0)
      || (read_be_uint32_from_file (fptr, &pvdd->type_m_path_table_location) != 0)
      || (read_be_uint32_from_file (fptr, &pvdd->optional_type_m_path_table_location) != 0)
      || (read_dir_rec_from_file (fptr, &pvdd->root_directory_entry) != 0)
      || (read_str_from_file (fptr, pvdd->volume_set_identifier, 128) != 0)
      || (read_str_from_file (fptr, pvdd->publisher_identifier, 128) != 0)
      || (read_str_from_file (fptr, pvdd->data_preparer_identifier, 128) != 0)
      || (read_str_from_file (fptr, pvdd->application_identifier, 128) != 0)
      || (read_str_from_file (fptr, pvdd->copyright_file_identifier, 37) != 0)
      || (read_str_from_file (fptr, pvdd->abstract_file_identifier, 37) != 0)
      || (read_str_from_file (fptr, pvdd->bibliographic_file_identifier, 37) != 0)
      || (read_pvd_date_time_from_file (fptr, &pvdd->volume_creation_date_time) != 0)
      || (read_pvd_date_time_from_file (fptr, &pvdd->volume_modification_date_time) != 0)
      || (read_pvd_date_time_from_file (fptr, &pvdd->volume_expiration_date_time) != 0)
      || (read_pvd_date_time_from_file (fptr, &pvdd->volume_effective_date_time) != 0)
      || (read_uint8_from_file (fptr, &pvdd->file_structure_version) != 0)
      || (read_uint8_array_from_file(fptr, pvdd->application_used_data, 512) != 0))
    return -1;
  /* clang-format on */

  return 0;
fseek_err:
  fprintf (stderr, "ERROR: failed to seek past unused byte(s) in primary "
                   "volume descriptor data.\n");
  return -1;
}

int8_t
read_le_be_uint32_from_file (FILE *fptr, uint32_t *output)
{
  if (read_le_uint32_from_file (fptr, output) != 0)
    return -1;

  uint32_t value = 0;
  if (read_be_uint32_from_file (fptr, &value) != 0)
    return -1;
  else if (*output != value)
    {
      fprintf (
          stderr,
          "ERROR: little endian 32-bit value read from file does not match "
          "the big endian value which follows it. Got %08X, expected %08X.\n",
          value, *output);
      return -1;
    }

  return 0;
}

int8_t
read_le_uint32_from_file (FILE *fptr, uint32_t *output)
{
  uint8_t bytes[4] = { 0 };
  for (size_t i = 0; i < 4; i++)
    {
      if (read_uint8_from_file (fptr, &bytes[i]) != 0)
        return -1;
    }

  *output = bytes[0];
  *output |= (uint32_t)(bytes[1] << 8);
  *output |= (uint32_t)(bytes[2] << 16);
  *output |= (uint32_t)(bytes[3] << 24);

  return 0;
}

int8_t
read_be_uint32_from_file (FILE *fptr, uint32_t *output)
{
  uint8_t bytes[4] = { 0 };
  for (size_t i = 0; i < 4; i++)
    {
      if (read_uint8_from_file (fptr, &bytes[i]) != 0)
        return -1;
    }

  *output = bytes[3];
  *output |= (uint32_t)(bytes[2] << 8);
  *output |= (uint32_t)(bytes[1] << 16);
  *output |= (uint32_t)(bytes[0] << 24);

  return 0;
}

int8_t
read_le_be_uint16_from_file (FILE *fptr, uint16_t *output)
{
  if (read_le_uint16_from_file (fptr, output) != 0)
    return -1;

  uint16_t value = 0;
  if (read_be_uint16_from_file (fptr, &value) != 0)
    return -1;
  else if (*output != value)
    {
      fprintf (
          stderr,
          "ERROR: little endian 16-bit value read from file does not match "
          "the big endian value which follows it. Got %04X, expected %04X.\n",
          value, *output);
      return -1;
    }

  return 0;
}

int8_t
read_le_uint16_from_file (FILE *fptr, uint16_t *output)
{
  uint8_t bytes[2] = { 0 };
  for (size_t i = 0; i < 2; i++)
    {
      if (read_uint8_from_file (fptr, &bytes[i]) != 0)
        return -1;
    }

  *output = bytes[0];
  *output |= (uint16_t)(bytes[1] << 8);

  return 0;
}

int8_t
read_be_uint16_from_file (FILE *fptr, uint16_t *output)
{
  uint8_t bytes[2] = { 0 };
  for (size_t i = 0; i < 2; i++)
    {
      if (read_uint8_from_file (fptr, &bytes[i]) != 0)
        return -1;
    }

  *output = bytes[1];
  *output |= (uint16_t)(bytes[0] << 8);

  return 0;
}

int8_t
read_dir_rec_from_file (FILE *fptr, Iso9660DirectoryRecord_t *dr)
{
  if ((read_uint8_from_file (fptr, &dr->dir_rec_length) != 0)
      || (read_uint8_from_file (fptr, &dr->extended_attrib_rec_length) != 0)
      || (read_le_be_uint32_from_file (fptr, &dr->extent_location) != 0)
      || (read_le_be_uint32_from_file (fptr, &dr->extent_size) != 0)
      || (read_uint8_from_file (fptr, &dr->recording_date_time.year) != 0)
      || (read_uint8_from_file (fptr, &dr->recording_date_time.month) != 0)
      || (read_uint8_from_file (fptr, &dr->recording_date_time.day) != 0)
      || (read_uint8_from_file (fptr, &dr->recording_date_time.hour) != 0)
      || (read_uint8_from_file (fptr, &dr->recording_date_time.minute) != 0)
      || (read_uint8_from_file (fptr, &dr->recording_date_time.second) != 0)
      || (read_uint8_from_file (fptr, &dr->recording_date_time.timezone) != 0)
      || (read_uint8_from_file (fptr, &dr->file_flags) != 0)
      || (read_uint8_from_file (fptr, &dr->file_unit_size) != 0)
      || (read_uint8_from_file (fptr, &dr->interleave_gap_size) != 0)
      || (read_le_be_uint16_from_file (fptr, &dr->volume_sequence_number) != 0)
      || (read_uint8_from_file (fptr, &dr->file_identifier_length) != 0)
      || (read_str_from_file (fptr, dr->file_identifier,
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
  if ((read_str_from_file (fptr, dt->year, 4) != 0)
      || (read_str_from_file (fptr, dt->month, 2) != 0)
      || (read_str_from_file (fptr, dt->day, 2) != 0)
      || (read_str_from_file (fptr, dt->hour, 2) != 0)
      || (read_str_from_file (fptr, dt->minute, 2) != 0)
      || (read_str_from_file (fptr, dt->second, 2) != 0)
      || (read_str_from_file (fptr, dt->hundredths_of_a_second, 2) != 0)
      || (read_uint8_from_file (fptr, &dt->timezone) != 0))
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
read_uint8_array_from_file (FILE *fptr, uint8_t *output, size_t length)
{
  size_t bytes_read = fread (output, sizeof (uint8_t), length, fptr);
  if (bytes_read != sizeof (uint8_t) * length)
    {
      fprintf (stderr,
               "ERROR: Error reading uint8 array of size %ld from file.\n",
               length);
      return -1;
    }

  return 0;
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
  puts (output_dir_path);

  /** Logical block size. Using this var for readability's sake. */
  uint16_t lb_size = fs->volume_desc_data.primary_vol_desc.logical_block_size;
  uint64_t path_table_loc
      = fs->volume_desc_data.primary_vol_desc.type_l_path_table_location
        * lb_size;

  if (fseek (input_fptr, path_table_loc, SEEK_SET) != 0)
    {
      fprintf (stderr, "ERROR: failed to seek to root directory (0x%08lX).\n",
               path_table_loc);
      return -1;
    }

  /** TODO: Save a list of every path table, then separately process each dr. */
  while (true)
    {
      PathTable_t path_table_entry;
      if (read_pt_from_file (input_fptr, &path_table_entry) != 0)
        return -1;

      if (path_table_entry.directory_identifier_length == 0)
        break;

      print_pt (&path_table_entry);

      if (fseek (input_fptr, path_table_entry.extent_location * lb_size,
                 SEEK_SET)
          != 0)
        {
          fprintf (stderr,
                   "ERROR: failed to seek to the directory record entry for "
                   "%.*s.\n",
                   path_table_entry.directory_identifier_length,
                   path_table_entry.directory_identifier);
          return -1;
        }

      Iso9660DirectoryRecord_t dr;
      read_dir_rec_from_file (input_fptr, &dr);
      print_dir_rec (&dr);

      break;
    }

  return 0;
}

int8_t
read_pt_from_file (FILE *fptr, PathTable_t *pt)
{
  if ((read_uint8_from_file (fptr, &pt->directory_identifier_length) != 0)
      || (read_uint8_from_file (fptr, &pt->extended_attribute_record_length)
          != 0)
      || (read_le_uint32_from_file (fptr, &pt->extent_location) != 0)
      || (read_le_uint16_from_file (fptr, &pt->parent_directory_number) != 0)
      || (read_str_from_file (fptr, pt->directory_identifier,
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
print_pt (PathTable_t *pt)
{
  printf ("%.*s - Ext. attrib. len: %d, Loc: %d, Parent: %d\n",
          pt->directory_identifier_length, pt->directory_identifier,
          pt->extended_attribute_record_length, pt->extent_location,
          pt->parent_directory_number);
}
