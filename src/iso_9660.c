/**
 *  Copyright (C) 2024-2025  IcePanorama
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "iso_9660.h"
#include "iso_9660/fs_header.h"
#include "iso_9660/pri_vol_desc.h"
#include "iso_9660/vol_desc_type_code.h"

#include <stdint.h>
#include <stdlib.h>

/** See:  https://wiki.osdev.org/ISO_9660. */
struct ISO9660FileSystem_s
{
  _FileSysHeader_t *header;

  /** See:  https://wiki.osdev.org/ISO_9660#Volume_Descriptors. */
  union _VolDescData_u
  {
    /** See:  https://wiki.osdev.org/ISO_9660#The_Boot_Record. */
    struct _BootRecVolDesc_s
    {
      char boot_system_identifier[32];
      char boot_identifier[32];
      uint8_t boot_system_data[1977]; // "Custom - used by the boot system."
    } boot_vol_desc;

    _PriVolDesc_t *pri_vol_desc;

    /**
     *  Volume Descriptor Set Terminator data "does not define bytes 7-2047 of
     *  its Volume Descriptor."
     *  See:  https://wiki.osdev.org/ISO_9660#Volume_Descriptor_Set_Terminator.
     */
    uint8_t vol_desc_set_term_data[2041];
  } vol_desc_data;
};

ISO9660FileSystem_t *
i9660_alloc (void)
{
  return calloc (1, sizeof (ISO9660FileSystem_t));
}

void
i9660_free (ISO9660FileSystem_t *fs)
{
  if (fs == NULL)
    return;

  switch (_fsh_get_vol_desc_type_code (fs->header))
    {
    case _VDTC_PRIMARY_VOLUME:
      _pvd_free (fs->vol_desc_data.pri_vol_desc);
      break;
    default:
      fprintf (stderr,
               "There is currently no support for freeing volume descriptor "
               "data of type %02X.\n",
               _fsh_get_vol_desc_type_code (fs->header));
      break;
    }

  _fsh_free (fs->header);
  free (fs);
}

void
i9660_print (ISO9660FileSystem_t *fs)
{
  if (fs == NULL)
    return;

  _fsh_print (fs->header);
  _VolDescTypeCode_t type = _fsh_get_vol_desc_type_code (fs->header);
  switch (type)
    {
    case _VDTC_PRIMARY_VOLUME:
      _pvd_print (fs->vol_desc_data.pri_vol_desc);
      break;
    default:
      fprintf (stderr,
               "There is currently no support for printing volume descriptor "
               "data of type %02X.\n",
               type);
      break;
    }
}

int
i9660_init (ISO9660FileSystem_t *fs, FILE input_fptr[static 1])
{
  if (fs == NULL)
    return -1;

  // See:  https://wiki.osdev.org/ISO_9660#System_Area.
  if (fseek (input_fptr, 0x8000, SEEK_SET) != 0)
    {
      fprintf (stderr, "ERROR: failed to seek past system area.\n");
      return -1;
    }

  fs->header = _fsh_alloc ();
  if ((fs->header == NULL) || (_fsh_init (fs->header, input_fptr) != 0))
    return -1;

  union _VolDescData_u *data = &fs->vol_desc_data;
  _VolDescTypeCode_t type = _fsh_get_vol_desc_type_code (fs->header);
  switch (type)
    {
    case _VDTC_PRIMARY_VOLUME:
      data->pri_vol_desc = _pvd_alloc ();
      if ((data->pri_vol_desc == NULL)
          || (_pvd_init (data->pri_vol_desc, input_fptr) != 0))
        return -1;
      break;
    default:
      fprintf (stderr,
               "There is currently no support for initializing volume "
               "descriptor data of type %02X.\n",
               type);
      return -1;
    }

  return 0;
}

int
i9660_extract (ISO9660FileSystem_t *fs, FILE input_fptr[static 1],
               const char path[static 1])
{
  if (fs == NULL)
    return -1;

  _VolDescTypeCode_t type = _fsh_get_vol_desc_type_code (fs->header);
  switch (type)
    {
    case _VDTC_PRIMARY_VOLUME:
      if (_pvd_extract (fs->vol_desc_data.pri_vol_desc, input_fptr, path) != 0)
        return -1;
      break;
    default:
      fprintf (stderr,
               "There is currently no support for extracting volume "
               "descriptor data of type %02X.\n",
               type);
      return -1;
    }

  return 0;
}
