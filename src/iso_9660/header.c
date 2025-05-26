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
#include "iso_9660/header.h"
#include "iso_9660/binary_reader.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/** See: https://wiki.osdev.org/ISO_9660#Volume_Descriptors. */
struct _ISO9660Header_s
{
  _ISO9660VolDescTypeCode_t vol_desc_type_code;

  char vol_id[5]; // Should always be `CD001`.
  uint8_t vol_desc_ver;
};

_ISO9660Header_t *
_i9660h_alloc (void)
{
  return calloc (1, sizeof (_ISO9660Header_t));
}

void
_i9660h_free (_ISO9660Header_t *h)
{
  if (h == NULL)
    return;

  free (h);
}

void
_i9660h_print (_ISO9660Header_t *h)
{
  if (h == NULL)
    return;

  printf ("Volume descriptor type code: %02X\n", h->vol_desc_type_code);
  printf ("Volume identifier: %.*s\n", 5, h->vol_id);
  printf ("Volume descriptor version: %02X\n", h->vol_desc_ver);
}

/**
 *  Reads a volume descriptor type code from file.
 *  Returns: zero on success, non-zero on failure.
 *  See: `_ISO9660VolDescTypeCode_t`
 *  FIXME: move to vdtc file?
 */
static int
read_vd_type_code (FILE *fptr, _ISO9660VolDescTypeCode_t *output)
{
  uint8_t byte;
  if (_br_read_u8 (fptr, &byte) != 0)
    return -1;

  if ((byte > _VDTC_VOL_PARTITION) && (byte != _VDTC_VOL_DESC_SET_TERMINATOR))
    {
      fprintf (stderr, "Unknown volume descriptor type code: %02X.\n", byte);
      return -1;
    }

  *output = (_ISO9660VolDescTypeCode_t)(byte);
  return 0;
}

int
_i9660h_init (_ISO9660Header_t *h, FILE input_fptr[static 1])
{
  if (h == NULL)
    return -1;

  if ((read_vd_type_code (input_fptr, &h->vol_desc_type_code) != 0)
      || (_br_read_str (input_fptr, h->vol_id, 5) != 0))
    return -1;

  if (strncmp ("CD001", h->vol_id, 5) != 0)
    {
      fprintf (stderr, "Incorrect file signature: expected %s, got %s.\n",
               "CD001", h->vol_id);
      return -1;
    }

  if (_br_read_u8 (input_fptr, &h->vol_desc_ver))
    return -1;

  return 0;
}

_ISO9660VolDescTypeCode_t
_i9660h_get_vol_desc_type_code (_ISO9660Header_t *h)
{
  if (h == NULL)
    return _VDTC_VOL_DESC_SET_TERMINATOR;

  return h->vol_desc_type_code;
}
