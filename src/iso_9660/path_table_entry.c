#include "iso_9660/path_table_entry.h"
#include "iso_9660/binary_reader.h"
#include "iso_9660/dir_rec.h"
#include "iso_9660/file_flags.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct _PathTableEntry_s
{
  uint8_t dir_id_len;
  uint8_t extended_attribute_record_len;
  uint32_t extent_loc;
  uint16_t parent_dir_num;
  char dir_id[UINT8_MAX]; // See `dir_id_len`.
};

const size_t _PathTableEntry_SIZE_BYTES = sizeof (_PathTableEntry_t);

_PathTableEntry_t *
_pte_alloc (void)
{
  return calloc (1, sizeof (_PathTableEntry_t));
}

void
_pte_free (_PathTableEntry_t *p)
{
  if (p == NULL)
    return;

  free (p);
}

uint8_t
_pte_get_dir_id_len (const _PathTableEntry_t *e)
{
  if (e == NULL)
    return 0;

  return e->dir_id_len;
}

uint16_t
_pte_get_parent_dir_num (const _PathTableEntry_t *e)
{
  if (e == NULL)
    return 1;

  return e->parent_dir_num;
}

const char *
_pte_get_dir_id (const _PathTableEntry_t *e)
{
  if (e == NULL)
    return NULL;

  return e->dir_id;
}

int
_pte_init (_PathTableEntry_t *p, FILE input_fptr[static 1])
{
  if (p == NULL)
    return -1;

  if ((_i9660br_read_u8 (input_fptr, &p->dir_id_len) != 0)
      || (_i9660br_read_u8 (input_fptr, &p->extended_attribute_record_len)
          != 0)
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
_pte_print (_PathTableEntry_t *p)
{
  if (p == NULL)
    return;

  printf ("%.*s - Ext. attrib. len: %d, Loc: %d, Parent: %d\n", p->dir_id_len,
          p->dir_id, p->extended_attribute_record_len, p->extent_loc,
          p->parent_dir_num);
}

int
_pte_extract (_PathTableEntry_t *p, uint16_t lb_size,
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

  // Extract every directory record associated with this entry
  size_t dr_list_capacity = 1;
  size_t dr_list_len = 0;
  _DirRec_t *dr_list = calloc (dr_list_capacity, _dr_size ());
  if (dr_list == NULL)
    {
      fprintf (stderr, "%s: Out of memory error.\n", __func__);
      return -1;
    }

  int ret = _dr_dynamic_init (&dr_list, &dr_list_capacity, &dr_list_len,
                              lb_size, input_fptr);
  if (ret != 0)
    goto err_exit;

  for (size_t i = 0; i < dr_list_len; i++)
    {
      _DirRec_t *curr = (_DirRec_t *)((char *)dr_list + (i * _dr_size ()));

      // Skip directories. See: https://wiki.osdev.org/ISO_9660#Directories.
      if (_dr_get_flags (curr) & (1 << (_FF_IS_DIRECTORY_BIT)))
        continue;

      if (_dr_extract (curr, lb_size, input_fptr, path, path_len) != 0)
        goto err_exit;
    }

  free (dr_list);
  return 0;
err_exit:
  free (dr_list);
  return -1;
}
