#include "iso_9660/path_table_entry.h"
#include "iso_9660/binary_reader.h"

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
