#ifndef _ISO_9660_FILESYSTEM_HEADER_H_
#define _ISO_9660_FILESYSTEM_HEADER_H_

#include <stdint.h>
#include <stdio.h>

typedef struct _FileSysHeader_s
{
  /** See: https://wiki.osdev.org/ISO_9660#Volume_Descriptor_Type_Codes. */
  enum _VolDescTypeCode_e
  {
    VDTC_BOOT_RECORD,
    VDTC_PRIMARY_VOLUME,
    VDTC_SUPPLEMENTARY_VOL,
    VDTC_VOL_PARTITION,
    VDTC_VOL_DESC_SET_TERMINATOR = 255
  } vol_desc_type_code;

  char vol_id[5]; // Always `CD001`.
  uint8_t vol_desc_ver;
} _FileSysHeader_t;

int _fs_header_init (_FileSysHeader_t *h, FILE *input_fptr);
void _fs_header_print (_FileSysHeader_t *h);

#endif /* _ISO_9660_FILESYSTEM_HEADER_H_ */
