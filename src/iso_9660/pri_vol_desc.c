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
#include "iso_9660/pri_vol_desc.h"
#include "iso_9660/binary_reader.h"
#include "iso_9660/dir_rec.h"
#include "iso_9660/path_table_entry.h"
#include "iso_9660/pri_vol_date_time.h"
#include "iso_9660/utils.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/** See: https://wiki.osdev.org/ISO_9660#The_Primary_Volume_Descriptor. */
struct _ISO9660PriVolDesc_s
{
  char sys_id[32];
  char vol_id[32];

  /** "number of logical blocks in which the volume is recorded." */
  uint32_t vol_space_size;
  uint16_t vol_set_size; // "number of disks."
  uint16_t vol_seq_num;  // number of this disk in the set.

  uint16_t logical_blk_size; // Used for logical block addressing below.
  uint32_t path_table_size;
  uint32_t type_l_path_table_loc;     // little-endian path table
  uint32_t opt_type_l_path_table_loc; // optional
  uint32_t type_m_path_table_loc;     // big-endian path table
  uint32_t opt_type_m_path_table_loc; // optional

  _ISO9660DirRec_t *root_directory_entry;

  char vol_set_id[128];
  char publisher_id[128];
  char data_preparer_id[128];
  char application_id[128];
  char copyright_file_id[37];
  char abstract_file_id[37];
  char bibliographic_file_id[37];

  _ISO9660PVDDateTime_t *creation_date_time;
  _ISO9660PVDDateTime_t *modification_date_time;
  _ISO9660PVDDateTime_t *expiration_date_time;
  _ISO9660PVDDateTime_t *effective_date_time;

  uint8_t fs_ver; // Should always be `0x01`.
  uint8_t application_used_data[512];

  /*
   *  The following three data fields are not included in specification, but
   *  are used internally for extracting primary volume descriptor data.
   */
  _ISO9660PathTableEntry_t *pt_list;
  size_t pt_list_len;      // logical size of `pt_list`
  size_t pt_list_capacity; // physical size of `pt_list`
};

_ISO9660PriVolDesc_t *
_i9660pvd_alloc (void)
{
  return calloc (1, sizeof (_ISO9660PriVolDesc_t));
}

void
_i9660pvd_free (_ISO9660PriVolDesc_t *p)
{
  if (p == NULL)
    return;

  if (p->root_directory_entry != NULL)
    _i9660dr_free (p->root_directory_entry);

  if (p->pt_list != NULL)
    {
      free (p->pt_list);
      p->pt_list = NULL;
    }

  /*
   *  `_i9660pvddt_free` will behave well on NULL inputs. Checking in advance,
   *  however, allows us to avoid the penality associated with an additional,
   *  unnecessary function call (at the expense of a potential branch
   *  misprediction penalty). Checking makes more sense logically, in my
   *  opinion. Totally unnecesary and needlessly complicated? Almost certainly.
   */
  if (p->creation_date_time != NULL)
    _i9660pvddt_free (p->creation_date_time);
  if (p->modification_date_time != NULL)
    _i9660pvddt_free (p->modification_date_time);
  if (p->expiration_date_time != NULL)
    _i9660pvddt_free (p->expiration_date_time);
  if (p->effective_date_time != NULL)
    _i9660pvddt_free (p->effective_date_time);

  free (p);
}

void
_i9660pvd_print (_ISO9660PriVolDesc_t *p)
{
  if (p == NULL)
    return;

  printf ("System identifier: %.*s\n", 32, p->sys_id);
  printf ("Volume identifier: %.*s\n", 32, p->vol_id);
  printf ("Volume space size: %d\n", p->vol_space_size);
  printf ("Volume set size: %d\n", p->vol_set_size);
  printf ("Volume sequence number: %d\n", p->vol_seq_num);

  printf ("Logical block size: %d\n", p->logical_blk_size);
  printf ("Path table size: %d\n", p->path_table_size);
  printf ("Location of type-l path table: %d\n", p->type_l_path_table_loc);
  printf ("Location of optional type-l path table: %d\n",
          p->opt_type_l_path_table_loc);
  printf ("Location of type-m path table: %d\n", p->type_m_path_table_loc);
  printf ("Location of optional type-m path table: %d\n",
          p->opt_type_m_path_table_loc);

  puts ("Root directory entry:");
  _i9660dr_print (p->root_directory_entry);

  printf ("Volume set identifier: %.*s\n", 128, p->vol_set_id);
  printf ("Publisher identifier: %.*s\n", 128, p->publisher_id);
  printf ("Data preparer identifier: %.*s\n", 128, p->data_preparer_id);
  printf ("Application identifier: %.*s\n", 128, p->application_id);
  printf ("Copyright file identifier: %.*s\n", 37, p->copyright_file_id);
  printf ("Abstract file identifier: %.*s\n", 37, p->abstract_file_id);
  printf ("Bibliographic file identifier: %.*s\n", 37,
          p->bibliographic_file_id);

  _i9660pvddt_print (p->creation_date_time, "Volume creation date time");
  _i9660pvddt_print (p->modification_date_time,
                     "Volume modification date time");
  _i9660pvddt_print (p->expiration_date_time, "Volume expiration date time");
  _i9660pvddt_print (p->effective_date_time, "Volume effective date time");

  printf ("File structure version: %d\n", p->fs_ver);

  if (p->pt_list == NULL)
    return;

  puts ("Path table:");
  for (size_t i = 0; i < p->pt_list_len; i++)
    {
      _ISO9660PathTableEntry_t *curr
          = (_ISO9660PathTableEntry_t *)((char *)(p->pt_list)
                                         + (i * _I9660PTE_SIZE_BYTES));
      _i9660pte_print (curr);
    }
}

/**
 *  Wrapper for `_i9660pvddt_init` which handles necessary set up stuff.
 *
 *  Return: Zero on success, non-zero on failure.
 */
static int
handle_date_time_data (_ISO9660PVDDateTime_t **d, FILE *input_fptr)
{
  *d = _i9660pvddt_alloc ();
  if (*d == NULL)
    {
      fprintf (stderr, "%s: Out of memory error.\n", __func__);
      return -1;
    }
  else if (_i9660pvddt_init (*d, input_fptr) != 0)
    return -1;

  return 0;
}

/**
 *  Doubles the size of given primary volume descriptor data's path table list.
 *
 *  Return:  Zero on success, non-zero on failure.
 */
static int
resize_pt_list (_ISO9660PriVolDesc_t *p)
{
  p->pt_list_capacity *= 2;
  const size_t SIZE_BYTES = p->pt_list_capacity * _I9660PTE_SIZE_BYTES;
  _ISO9660PathTableEntry_t *tmp = realloc (p->pt_list, SIZE_BYTES);
  if (tmp == NULL)
    {
      fprintf (stderr, "Failed to grow path table list to size %ld.\n",
               p->pt_list_capacity);
      return -1;
    }

  p->pt_list = tmp;
  return 0;
}

/**
 *  Builds a path table list using data from `input_fptr`.
 *
 *  Return:  Zero on success, non-zero on failure.
 */
static int
process_path_table_list (_ISO9660PriVolDesc_t *p, FILE *input_fptr)
{
  uint32_t pt_start = p->logical_blk_size * p->type_l_path_table_loc;
  if (fseek (input_fptr, pt_start, SEEK_SET) != 0)
    {
      fprintf (stderr,
               "Failed to seek to type-l path table location (%08X).\n",
               pt_start);
      return -1;
    }

  const uint32_t PT_END = pt_start + p->path_table_size;
  for (uint32_t i = pt_start; i < PT_END;)
    {
      _ISO9660PathTableEntry_t *curr = _i9660pte_alloc ();
      if (curr == NULL)
        {
          fprintf (stderr, "%s: Out of memory error.\n", __func__);
          return -1;
        }

      if (_i9660pte_init (curr, input_fptr) != 0)
        {
          fprintf (stderr, "Path table procesing failed.\n");
          return -1;
        }

      if (p->pt_list_len == p->pt_list_capacity)
        {
          if (resize_pt_list (p) != 0)
            return -1;
        }

      _ISO9660PathTableEntry_t *entry
          = (_ISO9660PathTableEntry_t *)((char *)(p->pt_list)
                                         + (p->pt_list_len
                                            * _I9660PTE_SIZE_BYTES));
      memcpy (entry, curr, _I9660PTE_SIZE_BYTES);
      _i9660pte_free (curr);
      p->pt_list_len++;

      i = ftell (input_fptr);
      if (i == (uint32_t)-1)
        {
          fprintf (stderr, "ftell error while processing path table list.\n");
          return -1;
        }
    }

  return 0;
}

int
_i9660pvd_init (_ISO9660PriVolDesc_t *p, FILE input_fptr[static 1])
{
  if (p == NULL)
    return -1;

  if (fseek (input_fptr, 1, SEEK_CUR) != 0) // Unused byte
    goto fseek_err;

  if ((_i9660br_read_str (input_fptr, p->sys_id, 32) != 0)
      || (_i9660br_read_str (input_fptr, p->vol_id, 32) != 0))
    return -1;

  if (fseek (input_fptr, 8, SEEK_CUR) != 0) // Unused bytes
    goto fseek_err;

  if (_i9660br_read_le_be_u32 (input_fptr, &p->vol_space_size) != 0)
    return -1;

  if (fseek (input_fptr, 32, SEEK_CUR) != 0) // Unused bytes
    goto fseek_err;

  /* clang-format off */
  if ((_i9660br_read_le_be_u16 (input_fptr, &p->vol_set_size) != 0)
      || (_i9660br_read_le_be_u16 (input_fptr, &p->vol_seq_num) != 0)
      || (_i9660br_read_le_be_u16 (input_fptr, &p->logical_blk_size) != 0)
      || (_i9660br_read_le_be_u32 (input_fptr, &p->path_table_size) != 0)
      || (_i9660br_read_le_u32 (input_fptr, &p->type_l_path_table_loc) != 0)
      || (_i9660br_read_le_u32 (input_fptr, &p->opt_type_l_path_table_loc) != 0)
      || (_i9660br_read_be_u32 (input_fptr, &p->type_m_path_table_loc) != 0)
      || (_i9660br_read_be_u32 (input_fptr, &p->opt_type_m_path_table_loc) != 0))
    return -1;
  /* clang-format on */

  p->root_directory_entry = _i9660dr_alloc ();
  if ((p->root_directory_entry == NULL)
      || (_i9660dr_init (p->root_directory_entry, input_fptr) != 0)
      || (_i9660br_read_str (input_fptr, p->vol_set_id, 128) != 0)
      || (_i9660br_read_str (input_fptr, p->publisher_id, 128) != 0)
      || (_i9660br_read_str (input_fptr, p->data_preparer_id, 128) != 0)
      || (_i9660br_read_str (input_fptr, p->application_id, 128) != 0)
      || (_i9660br_read_str (input_fptr, p->copyright_file_id, 37) != 0)
      || (_i9660br_read_str (input_fptr, p->abstract_file_id, 37) != 0)
      || (_i9660br_read_str (input_fptr, p->bibliographic_file_id, 37) != 0))
    return -1;

  if ((handle_date_time_data (&p->creation_date_time, input_fptr) != 0)
      || (handle_date_time_data (&p->modification_date_time, input_fptr) != 0)
      || (handle_date_time_data (&p->expiration_date_time, input_fptr) != 0)
      || (handle_date_time_data (&p->effective_date_time, input_fptr) != 0))
    return -1;

  const uint8_t EXP_VER = 0x01; // See above.
  if (_i9660br_read_u8 (input_fptr, &p->fs_ver) != 0)
    return -1;
  else if (p->fs_ver != EXP_VER)
    {
      fprintf (stderr,
               "Incorrect file system version: expected %02X, got %02X.\n",
               EXP_VER, p->fs_ver);
      return -1;
    }

  if (_i9660br_read_u8_array (input_fptr, p->application_used_data, 512) != 0)
    return -1;

  p->pt_list_capacity = 1;
  p->pt_list = calloc (p->pt_list_capacity, _I9660PTE_SIZE_BYTES);
  if (p->pt_list == NULL)
    {
      fprintf (stderr, "Failed to alloc path table list.\n");
      return -1;
    }

  process_path_table_list (p, input_fptr);

  return 0;
fseek_err:
  fprintf (stderr, "ERROR: failed to seek past unused byte(s) in primary "
                   "volume descriptor data.\n");
  return -1;
}

/**
 *  Calculates how long the full path of a given entry (`entry_idx`) must be.
 */
static size_t
calc_entry_path_len (_ISO9660PriVolDesc_t p[static 1], size_t entry_idx,
                     const char path[static 1])
{
  size_t path_len = 0;
  ssize_t i = entry_idx;
  do
    {
      _ISO9660PathTableEntry_t *curr
          = (_ISO9660PathTableEntry_t *)((char *)(p->pt_list)
                                         + (i * _I9660PTE_SIZE_BYTES));
      path_len += _i9660pte_get_dir_id_len (curr) + 1; // + 1 for '/'
      i = _i9660pte_get_parent_dir_num (curr) - 1;
    }
  while (i > 0);

  path_len += strlen (path) + 1; // + 1 for '/'
  path_len += strcspn (p->vol_id, " ");
  path_len += 1;
  path_len++; // null terminator

  return path_len;
}

/**
 *  Builds path string (`output`) for a given entry (`entry_idx`).
 *
 *  Return: Zero on success, non-zero on failure.
 */
static int
build_entry_path_str (_ISO9660PriVolDesc_t p[static 1], char *output,
                      size_t output_len, size_t entry_idx,
                      const char path[static 1])
{
  if (output == NULL)
    return -1;

  size_t j = entry_idx;
  do
    {
      const _ISO9660PathTableEntry_t *curr
          = (_ISO9660PathTableEntry_t *)((char *)(p->pt_list)
                                         + (j * _I9660PTE_SIZE_BYTES));

      const char *id = _i9660pte_get_dir_id (curr);
      const uint8_t id_len = _i9660pte_get_dir_id_len (curr);
      if ((_i9660u_prepend_str (output, output_len, "/", 2) != 0)
          || (_i9660u_prepend_str (output, output_len, id, id_len) != 0))
        return -1;

      j = _i9660pte_get_parent_dir_num (curr) - 1;
    }
  while (j > 0);

  if (output[0] != '/') // when not dealing with root dir
    {
      if (_i9660u_prepend_str (output, output_len, "/", 2) != 0)
        return -1;
    }

  size_t vol_id_len = strcspn (p->vol_id, " ");
  const size_t path_len = strlen (path) + 1;
  if ((_i9660u_prepend_str (output, output_len, p->vol_id, vol_id_len) != 0)
      || (_i9660u_prepend_str (output, output_len, "/", 2) != 0)
      || (_i9660u_prepend_str (output, output_len, path, path_len) != 0))
    return -1;

  return 0;
}

int
_i9660pvd_extract (_ISO9660PriVolDesc_t *p, FILE input_fptr[static 1],
                   const char path[static 1])
{
  if (p == NULL)
    return -1;

  if (p->pt_list == NULL)
    {
      fprintf (stderr, "Can't extract primary volume descriptor: path table "
                       "list is null.\n");
      return -1;
    }

  for (size_t i = 0; i < p->pt_list_len; i++)
    {
      _ISO9660PathTableEntry_t *curr
          = (_ISO9660PathTableEntry_t *)((char *)(p->pt_list)
                                         + (i * _I9660PTE_SIZE_BYTES));

      /*
       *  `+ UINT8_MAX` for the file identifier, +1 for NULL-terminator.
       *  See: `_ISO9660DirRec_t`
       */
      size_t path_len = calc_entry_path_len (p, i, path) + UINT8_MAX + 1;
      char *entry_path = calloc (path_len, sizeof (char));
      if (entry_path == NULL)
        {
          const char *id = _i9660pte_get_dir_id (curr);
          const uint8_t id_len = _i9660pte_get_dir_id_len (curr);
          fprintf (stderr,
                   "Error building path string for path table entry, %.*s\n",
                   id_len, id);
          return -1;
        }

      if (build_entry_path_str (p, entry_path, path_len, i, path) != 0)
        {
          free (entry_path);
          return -1;
        }

      printf ("Extracting directory: %s\n", entry_path);
      // still need to free `entry_path` b4 handling potential errors.
      int ret = _i9660pte_extract (curr, p->logical_blk_size, input_fptr,
                                   entry_path, path_len);

      free (entry_path);
      if (ret != 0)
        return -1;
    }

  return 0;
}
