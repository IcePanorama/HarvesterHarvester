#include "binary_reader.h"

#include "iso_9660/filesystem.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
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
      if (extract_pvd_fs (input_fptr, &fs->volume_desc_data.primary_vol_desc,
                          output_dir_path)
          != 0)
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
