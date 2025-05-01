#include "iso_9660/fs_header.h"
#include "iso_9660/binary_reader.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct _FileSysHeader_s
{
  _VolDescTypeCode_t vol_desc_type_code;

  char vol_id[5]; // Should always be `CD001`.
  uint8_t vol_desc_ver;
};

_FileSysHeader_t *
_fsh_alloc (void)
{
  _FileSysHeader_t *out = calloc (1, sizeof (_FileSysHeader_t));
  return out;
}

void
_fsh_free (_FileSysHeader_t *h)
{
  if (h == NULL)
    return;

  free (h);
}

/**
 *  Reads a volume descriptor type code from file.
 *  Returns: zero on success, non-zero on failure.
 *  See: `enum VolumeDescriptorTypeCode_e`
 */
static int
read_vd_type_code (FILE *fptr, enum _VolDescTypeCode_e *output)
{
  uint8_t byte;
  if (_br_read_u8 (fptr, &byte) != 0)
    return -1;

  if ((byte > _VDTC_VOL_PARTITION) && (byte != _VDTC_VOL_DESC_SET_TERMINATOR))
    {
      fprintf (stderr, "Unknown volume descriptor type code: %02X.\n", byte);
      return -1;
    }

  *output = (enum _VolDescTypeCode_e) (byte);
  return 0;
}

int
_fsh_init (_FileSysHeader_t *h, FILE input_fptr[static 1])
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

void
_fsh_print (_FileSysHeader_t *h)
{
  if (h == NULL)
    return;

  printf ("Volume descriptor type code: %02X\n", h->vol_desc_type_code);
  printf ("Volume identifier: %.*s\n", 5, h->vol_id);
  printf ("Volume descriptor version: %02X\n", h->vol_desc_ver);
}

_VolDescTypeCode_t
_fsh_get_vol_desc_type_code (_FileSysHeader_t *h)
{
  if (h == NULL)
    return _VDTC_VOL_DESC_SET_TERMINATOR;

  return h->vol_desc_type_code;
}
