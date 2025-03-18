#ifndef _ISO_9660_FILESYSTEM_HEADER_H_
#define _ISO_9660_FILESYSTEM_HEADER_H_

#include <stdint.h>

typedef struct _FileSystem_Header_s
{
  /** @see: https://wiki.osdev.org/ISO_9660#Volume_Descriptor_Type_Codes */
  enum _VolDescTypeCode_e
  {
    VDTC_BOOT_RECORD,
    VDTC_PRIMARY_VOLUME,
    VDTC_SUPPLEMENTARY_VOL,
    VDTC_VOL_PARTITION,
    VDTC_VOL_DESC_SET_TERMINATOR = 255
  } volume_desc_type_code;

  char volume_id[5]; //!< Always `CD001`.
  uint8_t volume_desc_ver;
} _FileSystem_Header_t;

#endif /* _ISO_9660_FILESYSTEM_HEADER_H_ */
