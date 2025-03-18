#include "iso_9660/pri_vol_desc.h"
#include "iso_9660/binary_reader.h"
#include "iso_9660/dir_rec.h"

int
_pvd_init (_PriVolDesc_t p[static 1], FILE input_fptr[static 1])
{
  if (fseek (input_fptr, 1, SEEK_CUR) != 0) // Unused
    goto fseek_err;

  if ((_br_read_str (input_fptr, p->sys_id, 32) != 0)
      || (_br_read_str (input_fptr, p->vol_id, 32)))
    return -1;

  if (fseek (input_fptr, 8, SEEK_CUR) != 0) // Unused
    goto fseek_err;

  if (_br_read_le_be_u32 (input_fptr, &p->vol_space_size) != 0)
    return -1;

  if (fseek (input_fptr, 32, SEEK_CUR) != 0) // Unused
    goto fseek_err;

  /* clang-format off */
  if ((_br_read_le_be_u16 (input_fptr, &p->vol_set_size) != 0)
      || (_br_read_le_be_u16 (input_fptr, &p->vol_seq_num) != 0)
      || (_br_read_le_be_u16 (input_fptr, &p->logical_blk_size) != 0)
      || (_br_read_le_be_u32 (input_fptr, &p->path_table_size) != 0)
      || (_br_read_le_u32 (input_fptr, &p->type_l_path_table_loc) != 0)
      || (_br_read_le_u32 (input_fptr, &p->optional_type_l_path_table_loc) != 0)
      || (_br_read_be_u32 (input_fptr, &p->type_m_path_table_loc) != 0)
      || (_br_read_be_u32 (input_fptr, &p->optional_type_m_path_table_loc) != 0)
      || (_dr_init (&p->root_directory_entry, input_fptr) != 0)
      || (_br_read_str (input_fptr, p->vol_set_id, 128) != 0)
      || (_br_read_str (input_fptr, p->publisher_id, 128) != 0)
      || (_br_read_str (input_fptr, p->data_preparer_id, 128) != 0)
      || (_br_read_str (input_fptr, p->application_id, 128) != 0)
      || (_br_read_str (input_fptr, p->copyright_file_id, 37) != 0)
      || (_br_read_str (input_fptr, p->abstract_file_id, 37) != 0)
      || (_br_read_str (input_fptr, p->bibliographic_file_id, 37) != 0)
      || (_pvddt_init (&p->creation_date_time, input_fptr) != 0)
      || (_pvddt_init (&p->modification_date_time, input_fptr) != 0)
      || (_pvddt_init (&p->expiration_date_time, input_fptr) != 0)
      || (_pvddt_init (&p->effective_date_time, input_fptr) != 0)
      || (_br_read_u8 (input_fptr, &p->fs_ver) != 0)
      || (_br_read_u8_array(input_fptr, p->application_used_data, 512) != 0))
    return -1;
  /* clang-format on */

  return 0;
fseek_err:
  fprintf (stderr, "ERROR: failed to seek past unused byte(s) in primary "
                   "volume descriptor data.\n");
  return -1;
}

void
_pvd_print (_PriVolDesc_t p[static 1])
{
  printf ("System identifier: %.*s\n", 32, p->sys_id);
  printf ("Volume identifier: %.*s\n", 32, p->vol_id);
  printf ("Volume space size: %d\n", p->vol_space_size);
  printf ("Volume set size: %d\n", p->vol_set_size);
  printf ("Volume sequence number: %d\n", p->vol_seq_num);
  printf ("Logical block size: %d\n", p->logical_blk_size);

  printf ("Path table size: %d\n", p->path_table_size);
  printf ("Location of type-l path table: 0x%08X\n", p->type_l_path_table_loc);
  printf ("Location of optional type-l path table: 0x%08X\n",
          p->optional_type_l_path_table_loc);
  printf ("Location of type-m path table: 0x%08X\n", p->type_m_path_table_loc);
  printf ("Location of optional type-m path table: 0x%08X\n",
          p->optional_type_m_path_table_loc);

  puts ("Root directory entry:");
  _dr_print (&p->root_directory_entry);

  printf ("Volume set identifier: %.*s\n", 128, p->vol_set_id);
  printf ("Publisher identifier: %.*s\n", 128, p->publisher_id);
  printf ("Data preparer identifier: %.*s\n", 128, p->data_preparer_id);
  printf ("Application identifier: %.*s\n", 128, p->application_id);
  printf ("Copyright file identifier: %.*s\n", 37, p->copyright_file_id);
  printf ("Abstract file identifier: %.*s\n", 37, p->abstract_file_id);
  printf ("Bibliographic file identifier: %.*s\n", 37,
          p->bibliographic_file_id);

  _pvddt_print (&p->creation_date_time, "Volume creation date time");
  _pvddt_print (&p->modification_date_time, "Volume modification date time");
  _pvddt_print (&p->expiration_date_time, "Volume expiration date time");
  _pvddt_print (&p->effective_date_time, "Volume effective date time");

  printf ("File structure version: %d\n", p->fs_ver);
}
