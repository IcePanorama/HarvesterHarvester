#include "iso_9660/filesystem.h"
#include "iso_9660/fs_header.h"
#include "iso_9660/pri_vol_desc.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/** See: https://wiki.osdev.org/ISO_9660 */
struct ISO9660FileSystem_s
{
  _FileSystem_Header_t header;

  /** See: https://wiki.osdev.org/ISO_9660#Volume_Descriptors */
  union _VolDescData_u
  {
    /** See: https://wiki.osdev.org/ISO_9660#The_Boot_Record */
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

  free (fs);
}

int
i9660_init_fs (ISO9660FileSystem_t *fs, FILE fptr[static 1])
{
  if (fs == NULL)
    return -1;

  // tmp
  printf ("%ld\n", ftell (fptr));

  return 0;
}
