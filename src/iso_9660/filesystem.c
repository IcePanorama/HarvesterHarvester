#include "iso_9660/filesystem.h"

#include <stdint.h>
#include <stdlib.h>

/** @see: https://wiki.osdev.org/ISO_9660 */
struct ISO9660FileSystem_s
{
  /** @see: https://wiki.osdev.org/ISO_9660#Volume_Descriptor_Type_Codes */
  enum VolumeDescriptorTypeCode_e
  {
    VDTC_BOOT_RECORD,
    VDTC_PRIMARY_VOLUME,
    VDTC_SUPPLEMENTARY_VOL,
    VDTC_VOL_PARTITION,
    VDTC_VOL_DESC_SET_TERMINATOR = 255
  } volume_desc_type_code;

  char volume_identifier[5]; //!< Always `CD001`.
  uint8_t volume_desc_version_num;

  /** @see: https://wiki.osdev.org/ISO_9660#Volume_Descriptors */
  union VolumeDescriptorData_u
  {
    /** @see: https://wiki.osdev.org/ISO_9660#The_Boot_Record */
    struct BootRecordVolumeDescriptorData_s
    {
      char boot_system_identifier[32];
      char boot_identifier[32];
      uint8_t boot_system_data[1977]; //!< "Custom - used by the boot system."
    } boot_vol_desc;

    /*
    PriVolDescrData_t primary_vol_desc;
     */

    /**
     *  Volume Descriptor Set Terminator data "does not define bytes 7-2047 of
     *  its Volume Descriptor."
     *  @see: https://wiki.osdev.org/ISO_9660#Volume_Descriptor_Set_Terminator
     *  TODO: decide: is it even worth creating a struct for this then?
     *  In theory, we could put this into a (u)int8_t array.
     */
  } volume_desc_data;
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
