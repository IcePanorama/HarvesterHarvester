#include "iso_9660.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int8_t read_uint8_from_file (FILE *fptr, uint8_t *output);
/** Read a volume descriptor type code from file. */
static int8_t
read_vd_type_code_from_file (FILE *fptr,
                             enum VolumeDescriptorTypeCode_e *output);
static void print_iso_9660_fs (Iso9660FileSystem_t *fs);
static int8_t read_string_from_file (FILE *fptr, char *output, size_t length);
static int8_t
read_primary_vd_data_from_file (FILE *fptr,
                                struct PrimaryVolumeDescriptorData_s *pvdd);
static void print_primary_vd_data (struct PrimaryVolumeDescriptorData_s *pvdd);
/**
 *  Reads a little-endian followed by big-endian encoded unsigned 32-bit
 *  integer. Double checks that these values match because why not?
 */
static int8_t read_le_be_uint32_from_file (FILE *fptr, uint32_t *output);
static int8_t read_le_uint32_from_file (FILE *fptr, uint32_t *output);
static int8_t read_be_uint32_from_file (FILE *fptr, uint32_t *output);
static int8_t read_le_be_uint16_from_file (FILE *fptr, uint16_t *output);
static int8_t read_le_uint16_from_file (FILE *fptr, uint16_t *output);
static int8_t read_be_uint16_from_file (FILE *fptr, uint16_t *output);

int8_t
create_iso_9660_filesystem_from_file (FILE fptr[static 1],
                                      Iso9660FileSystem_t fs[static 1])
{
  memset (fs, 0, sizeof (Iso9660FileSystem_t));
  if (fseek (fptr, 0x8000, SEEK_SET) != 0)
    {
      fprintf (stderr, "ERROR: failed to seek past system area (32KiB).\n");
      return -1;
    }

  if ((read_vd_type_code_from_file (fptr, &fs->volume_desc_type_code) != 0)
      || (read_string_from_file (fptr, fs->volume_identifier, 5) != 0)
      || (read_uint8_from_file (fptr, &fs->volume_desc_version_num)))
    return -1;

  switch (fs->volume_desc_type_code)
    {
    case VDTC_PRIMARY_VOLUME:
      if (read_primary_vd_data_from_file (
              fptr, &fs->volume_desc_data.primary_vol_desc)
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
      print_primary_vd_data (&fs->volume_desc_data.primary_vol_desc);
      break;
    default:
      break;
    }
}

int8_t
read_string_from_file (FILE *fptr, char *output, size_t length)
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
read_primary_vd_data_from_file (FILE *fptr,
                                struct PrimaryVolumeDescriptorData_s *pvdd)
{
  if (fseek (fptr, 1, SEEK_CUR) != 0) // Unused
    goto fseek_err;

  if ((read_string_from_file (fptr, pvdd->system_identifier, 32) != 0)
      || (read_string_from_file (fptr, pvdd->volume_identifier, 32)))
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
      || (read_le_uint32_from_file (fptr, &pvdd->type_l_path_table_location) != 0))
    return -1;
  /* clang-format on */

  return 0;
fseek_err:
  fprintf (stderr, "ERROR: failed to seek past unused byte(s) in primary "
                   "volume descriptor data.\n");
  return -1;
}

void
print_primary_vd_data (struct PrimaryVolumeDescriptorData_s *pvdd)
{
  printf ("- System identifier: %.*s\n", 32, pvdd->system_identifier);
  printf ("- Volume identifier: %.*s\n", 32, pvdd->volume_identifier);
  printf ("- Volume space size: %d\n", pvdd->volume_space_size);
  printf ("- Volume set size: %d\n", pvdd->volume_set_size);
  printf ("- Volume sequence number: %d\n", pvdd->volume_sequence_number);
  printf ("- Logical block size: %d\n", pvdd->logical_block_size);
  printf ("- Path table size: %d\n", pvdd->path_table_size);
  printf ("- Location of Type-L path table: 0x%08X\n",
          pvdd->type_l_path_table_location);
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
