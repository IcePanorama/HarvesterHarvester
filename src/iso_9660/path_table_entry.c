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
#include "iso_9660/path_table_entry.h"
#include "iso_9660/binary_reader.h"
#include "iso_9660/dir_rec.h"
#include "iso_9660/file_flags.h"

#include <stdlib.h>
#include <string.h>

struct _I9660PathTableEntry_s
{
  uint8_t dir_id_len;
  uint8_t ext_attrib_rec_len;
  uint32_t extent_loc;
  uint16_t parent_dir_num;
  char dir_id[UINT8_MAX]; // See `dir_id_len`.
};

const size_t _I9660PTE_SIZE_BYTES = sizeof (_ISO9660PathTableEntry_t);

_ISO9660PathTableEntry_t *
_i9660pte_alloc (void)
{
  return calloc (1, sizeof (_ISO9660PathTableEntry_t));
}

void
_i9660pte_free (_ISO9660PathTableEntry_t *p)
{
  if (p == NULL)
    return;

  free (p);
}

uint8_t
_i9660pte_get_dir_id_len (const _ISO9660PathTableEntry_t *p)
{
  if (p == NULL)
    return 0;

  return p->dir_id_len;
}

uint16_t
_i9660pte_get_parent_dir_num (const _ISO9660PathTableEntry_t *p)
{
  if (p == NULL)
    return 1;

  return p->parent_dir_num;
}

const char *
_i9660pte_get_dir_id (const _ISO9660PathTableEntry_t *p)
{
  if (p == NULL)
    return NULL;

  return p->dir_id;
}

int
_i9660pte_init (_ISO9660PathTableEntry_t *p, FILE input_fptr[static 1])
{
  if (p == NULL)
    return -1;

  if ((_i9660br_read_u8 (input_fptr, &p->dir_id_len) != 0)
      || (_i9660br_read_u8 (input_fptr, &p->ext_attrib_rec_len) != 0)
      || (_i9660br_read_le_u32 (input_fptr, &p->extent_loc) != 0)
      || (_i9660br_read_le_u16 (input_fptr, &p->parent_dir_num) != 0)
      || (_i9660br_read_str (input_fptr, p->dir_id, p->dir_id_len) != 0))
    {
      return -1;
    }

  if ((p->dir_id_len % 2) != 0)
    {
      if (fseek (input_fptr, 1, SEEK_CUR) != 0)
        {
          fprintf (stderr,
                   "ERROR: failed to seek past padding after path table.\n");
          return -1;
        }
    }

  return 0;
}

void
_i9660pte_print (_ISO9660PathTableEntry_t *p)
{
  if (p == NULL)
    return;

  printf ("%.*s - Ext. attrib. len: %d, Loc: %d, Parent: %d\n", p->dir_id_len,
          p->dir_id, p->ext_attrib_rec_len, p->extent_loc, p->parent_dir_num);
}

int
_i9660pte_extract (_ISO9660PathTableEntry_t *p, uint16_t lb_size,
                   FILE input_fptr[static 1], const char path[static 1],
                   const size_t path_len)
{
  if (p == NULL)
    return -1;

  if (fseek (input_fptr, lb_size * p->extent_loc, SEEK_SET) != 0)
    {
      fprintf (stderr,
               "Failed to seek to extant containing directory, %*.s.\n",
               p->dir_id_len, p->dir_id);
      return -1;
    }

  size_t dr_list_capacity = 1;
  size_t dr_list_len = 0;
  _ISO9660DirRec_t *dr_list = calloc (dr_list_capacity, _I9660DR_SIZE_BYTES);
  if (dr_list == NULL)
    {
      fprintf (stderr, "%s: Out of memory error.\n", __func__);
      return -1;
    }

  int ret = _i9660dr_dynamic_init (&dr_list, &dr_list_capacity, &dr_list_len,
                                   lb_size, input_fptr);
  if (ret != 0)
    goto err_exit;

  const size_t DR_SIZE = _I9660DR_SIZE_BYTES;
  for (size_t i = 0; i < dr_list_len; i++)
    {
      _ISO9660DirRec_t *curr
          = (_ISO9660DirRec_t *)((char *)dr_list + (i * DR_SIZE));

      if (_i9660dr_get_flags (curr) & (1 << (_I9660FF_IS_DIRECTORY_BIT)))
        continue;

      if (_i9660dr_extract (curr, lb_size, input_fptr, path, path_len) != 0)
        goto err_exit;
    }

  free (dr_list);
  return 0;
err_exit:
  free (dr_list);
  return -1;
}
