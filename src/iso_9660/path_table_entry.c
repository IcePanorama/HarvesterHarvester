#include "iso_9660/path_table_entry.h"
#include "iso_9660/binary_reader.h"
#include "iso_9660/dir_rec.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
_pte_init (_PathTableEntry_t p[static 1], FILE input_fptr[static 1])
{
  if ((_br_read_u8 (input_fptr, &p->dir_id_len) != 0)
      || (_br_read_u8 (input_fptr, &p->extended_attribute_record_len) != 0)
      || (_br_read_le_u32 (input_fptr, &p->extent_loc) != 0)
      || (_br_read_le_u16 (input_fptr, &p->parent_dir_num) != 0)
      || (_br_read_str (input_fptr, p->dir_id, p->dir_id_len) != 0))
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
_pte_print (_PathTableEntry_t p[static 1])
{
  printf ("%.*s - Ext. attrib. len: %d, Loc: %d, Parent: %d\n", p->dir_id_len,
          p->dir_id, p->extended_attribute_record_len, p->extent_loc,
          p->parent_dir_num);
}

int
_pte_extract (_PathTableEntry_t p[static 1], uint16_t lb_size,
              FILE input_fptr[static 1], const char path[static 1])
{
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
  _DirRec_t *dr_list = calloc (dr_list_capacity, sizeof (_DirRec_t));
  int ret = _dr_dynamic_init (&dr_list, &dr_list_capacity, &dr_list_len,
                              input_fptr);
  if (ret != 0)
    goto err_exit;

  for (size_t i = 0; i < dr_list_len; i++)
    {
      // Skip directories. See: https://wiki.osdev.org/ISO_9660#Directories.
      if (dr_list[i].file_flags & 2)
        continue;

      // Minus 2 to remove the ";1" from the end.
      printf ("%.*s\n", dr_list[i].file_id_len - 2, dr_list[i].file_id);
      _dr_print (&dr_list[i]);
      /*
      if (_dr_extract(&dr_list[i], input_fptr, path) != 0)
        goto err_exit;

      puts ("---");
      */
    }

  free (dr_list);
  return 0;
err_exit:
  free (dr_list);
  return -1;
  // tmp, prevent unused path param err
  puts (path);
  printf ("%ld\n", ftell (input_fptr));
}
