#include "iso_9660/filesystem.h"
#include "iso_9660/fs_header.h"
#include "iso_9660/pri_vol_desc.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/** See: https://wiki.osdev.org/ISO_9660. */
struct ISO9660FileSystem_s
{
  _FileSysHeader_t *header;

  /** See: https://wiki.osdev.org/ISO_9660#Volume_Descriptors. */
  union _VolDescData_u
  {
    /** See: https://wiki.osdev.org/ISO_9660#The_Boot_Record. */
    struct _BootRecVolDesc_s
    {
      char boot_system_identifier[32];
      char boot_identifier[32];
      uint8_t boot_system_data[1977]; // "Custom - used by the boot system."
    } boot_vol_desc;

    _PriVolDesc_t pri_vol_desc;

    /**
     *  Volume Descriptor Set Terminator data "does not define bytes 7-2047 of
     *  its Volume Descriptor."
     *  See: https://wiki.osdev.org/ISO_9660#Volume_Descriptor_Set_Terminator
     *  TODO: decide: is it even worth creating a struct for this then?
     *  In theory, we could put this into a (u)int8_t array.
     */
  } vol_desc_data;
};

ISO9660FileSystem_t *
i9660_create_fs (void)
{
  ISO9660FileSystem_t *output = calloc (1, sizeof (ISO9660FileSystem_t));
  return output;
}

void
i9660_free_fs (ISO9660FileSystem_t *fs)
{
  if (fs == NULL)
    return;

  switch (_fsh_get_vol_desc_type_code (fs->header))
    {
    case _VDTC_PRIMARY_VOLUME:
      _pvd_free (&fs->vol_desc_data.pri_vol_desc);
      break;
    default:
      break;
    }

  if (fs->header != NULL)
    _fsh_free (fs->header);

  free (fs);
}

static int
process_vol_desc_data (enum _VolDescTypeCode_e type,
                       union _VolDescData_u *data, FILE *input_fptr)
{
  switch (type)
    {
    case _VDTC_PRIMARY_VOLUME:
      if (_pvd_init (&data->pri_vol_desc, input_fptr) != 0)
        return -1;
      break;
    default:
      fprintf (
          stderr,
          "Support for initializing type code %02X isn't implemented yet.\n",
          type);
      return -1;
    }

  return 0;
}

int
i9660_init_fs (ISO9660FileSystem_t *fs, FILE input_fptr[static 1])
{
  if (fs == NULL)
    return -1;

  if (fseek (input_fptr, 0x8000, SEEK_SET) != 0)
    {
      fprintf (stderr, "ERROR: failed to seek past system area (32KiB).\n");
      return -1;
    }

  fs->header = _fsh_alloc ();
  if ((fs->header == NULL) || (_fsh_init (fs->header, input_fptr) != 0))
    return -1;

  if (process_vol_desc_data (_fsh_get_vol_desc_type_code (fs->header),
                             &fs->vol_desc_data, input_fptr)
      != 0)
    return -1;

  switch (_fsh_get_vol_desc_type_code (fs->header))
    {
    case _VDTC_PRIMARY_VOLUME:
      if (_pvd_process (&fs->vol_desc_data.pri_vol_desc, input_fptr) != 0)
        return -1;
      break;
    default:
      fprintf (stderr,
               "Support for processing type code %02X data isn't implemented "
               "yet.\n",
               _fsh_get_vol_desc_type_code (fs->header));
      return -1;
    }

  return 0;
}

void
i9660_print_fs (ISO9660FileSystem_t *fs)
{
  if (fs == NULL)
    return;

  _fsh_print (fs->header);
  switch (_fsh_get_vol_desc_type_code (fs->header))
    {
    case _VDTC_PRIMARY_VOLUME:
      _pvd_print (&fs->vol_desc_data.pri_vol_desc);
      break;
    default:
      fprintf (stderr,
               "Support for printing type code %02X isn't implemented yet.\n",
               _fsh_get_vol_desc_type_code (fs->header));
      break;
    }
}

int
i9660_extract_fs (ISO9660FileSystem_t *fs, FILE input_fptr[static 1],
                  const char path[static 1])
{
  if (fs == NULL)
    return -1;

  switch (_fsh_get_vol_desc_type_code (fs->header))
    {
    case _VDTC_PRIMARY_VOLUME:
      if (_pvd_extract (&fs->vol_desc_data.pri_vol_desc, input_fptr, path)
          != 0)
        return -1;
      break;
    default:
      fprintf (stderr,
               "No implemented support for extracting file systems of type "
               "code %02X.\n",
               _fsh_get_vol_desc_type_code (fs->header));
      return -1;
    }

  return 0;
}
