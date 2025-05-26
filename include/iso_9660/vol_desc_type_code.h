/**
 *  iso_9660/vol_desc_type_code.h - an interface for ISO 9660 volume descriptor
 *  type codes.
 *
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
#ifndef _ISO_9660_VOLUME_DESCRIPTOR_TYPE_CODE_H_
#define _ISO_9660_VOLUME_DESCRIPTOR_TYPE_CODE_H_

/** See: https://wiki.osdev.org/ISO_9660#Volume_Descriptor_Type_Codes. */
typedef enum _ISO9660FileSysVolDescTypeCode_e
{
  _VDTC_BOOT_RECORD,
  _VDTC_PRIMARY_VOLUME,
  _VDTC_SUPPLEMENTARY_VOL,
  _VDTC_VOL_PARTITION,
  _VDTC_VOL_DESC_SET_TERMINATOR = 255
} _ISO9660VolDescTypeCode_t;

#endif /* _ISO_9660_VOLUME_DESCRIPTOR_TYPE_CODE_H_ */
