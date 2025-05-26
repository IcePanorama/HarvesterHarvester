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
#include "iso_9660/dir_rec.h"
#include "iso_9660/binary_reader.h"
#include "iso_9660/utils.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/** See:: https://wiki.osdev.org/ISO_9660#Directories */
struct _ISO9660DirRec_s
{
  uint8_t len;
  uint8_t extended_attrib_rec_len;
  uint32_t extent_loc;
  uint32_t extent_size; // Also called its "data length".

  struct _DRDateTime_s
  {
    uint8_t year; // number of years since 1900.
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    /** "Offset from GMT in 15 minute intervals from -48 ... to +52." */
    uint8_t timezone;
  } recording_date_time;

  /**
   *  See: https://wiki.osdev.org/ISO_9660#Directories
   *  See: `_FileFlags_t`
   */
  _FileFlags_t file_flags;

  uint8_t file_unit_size;      // if recorded in interleaved mode, else 0.
  uint8_t interleave_gap_size; // if recorded in interleaved mode, else 0.
  uint16_t vol_seq_num;        // "the volume that this extent is recorded on."

  uint8_t file_id_len;
  /**
   *  Variable length file identifier. Actual length determined by
   *  `file_id_len`.
   *
   *  See: `file_id_len`.
   */
  char file_id[UINT8_MAX];
};

_ISO9660DirRec_t *
_i9660dr_alloc (void)
{
  return calloc (1, sizeof (_ISO9660DirRec_t));
}

void
_i9660dr_free (_ISO9660DirRec_t *dr)
{
  if (dr == NULL)
    return;

  free (dr);
}

void
_i9660dr_print (_ISO9660DirRec_t *dr)
{
  if (dr == NULL)
    return;

  printf ("Directory record length: %d\n", dr->len);
  printf ("Extended attribute length: %d\n", dr->extended_attrib_rec_len);
  printf ("Location of extent: %d\n", dr->extent_loc);
  printf ("Size of extent: %d\n", dr->extent_size);

  printf ("Recording date/time: %04d-%02d-%02d %02d:%02d:%02d (GMT%+02d)\n",
          1900 + dr->recording_date_time.year, dr->recording_date_time.month,
          dr->recording_date_time.day, dr->recording_date_time.hour,
          dr->recording_date_time.minute, dr->recording_date_time.second,
          (-48 + dr->recording_date_time.timezone) >> 2);

  puts ("File flags:");
  printf ("Hidden? %s\n", (dr->file_flags & (1 << (_I9660FF_HIDDEN_FILE_BIT)))
                              ? "true"
                              : "false");
  printf ("Directory? %s\n",
          (dr->file_flags & (1 << (_I9660FF_IS_DIRECTORY_BIT))) ? "true"
                                                                : "false");
  printf ("Associated file? %s\n",
          (dr->file_flags & (1 << (_I9660FF_IS_ASSOC_FILE_BIT))) ? "true"
                                                                 : "false");
  printf ("Format info in extended attrib. record? %s\n",
          (dr->file_flags & (1 << (_I9660FF_FMT_IN_EAR_BIT))) ? "true"
                                                              : "false");
  printf ("Owner/group perms in extended attrib. record? %s\n",
          (dr->file_flags & (1 << (_I9660FF_PERMS_IN_EAR_BIT))) ? "true"
                                                                : "false");
  printf ("Not final directory record? %s\n",
          (dr->file_flags & (1 << (_I9660FF_NOT_FINAL_DIR_BIT))) ? "true"
                                                                 : "false");

  printf ("File unit size: %d\n", dr->file_unit_size);
  printf ("Interleave gap size: %d\n", dr->interleave_gap_size);
  printf ("Volume sequence number: %d\n", dr->vol_seq_num);
  printf ("File identifier length: %d\n", dr->file_id_len);
  printf ("File identifier: %.*s\n", dr->file_id_len, dr->file_id);
}

int
_i9660dr_init (_ISO9660DirRec_t *dr, FILE input_fptr[static 1])
{
  if (dr == NULL)
    return -1;

  /* clang-format off */
  if ((_i9660br_read_u8 (input_fptr, &dr->len) != 0)
      || (_i9660br_read_u8 (input_fptr, &dr->extended_attrib_rec_len) != 0)
      || (_i9660br_read_le_be_u32 (input_fptr, &dr->extent_loc) != 0)
      || (_i9660br_read_le_be_u32 (input_fptr, &dr->extent_size) != 0)
      || (_i9660br_read_u8 (input_fptr, &dr->recording_date_time.year) != 0)
      || (_i9660br_read_u8 (input_fptr, &dr->recording_date_time.month) != 0)
      || (_i9660br_read_u8 (input_fptr, &dr->recording_date_time.day) != 0)
      || (_i9660br_read_u8 (input_fptr, &dr->recording_date_time.hour) != 0)
      || (_i9660br_read_u8 (input_fptr, &dr->recording_date_time.minute) != 0)
      || (_i9660br_read_u8 (input_fptr, &dr->recording_date_time.second) != 0)
      || (_i9660br_read_u8 (input_fptr, &dr->recording_date_time.timezone) != 0)
      || (_i9660br_read_u8 (input_fptr, &dr->file_flags) != 0)
      || (_i9660br_read_u8 (input_fptr, &dr->file_unit_size) != 0)
      || (_i9660br_read_u8 (input_fptr, &dr->interleave_gap_size) != 0)
      || (_i9660br_read_le_be_u16 (input_fptr, &dr->vol_seq_num) != 0)
      || (_i9660br_read_u8 (input_fptr, &dr->file_id_len) != 0)
      || (_i9660br_read_str (input_fptr, dr->file_id, dr->file_id_len) != 0))
    /* clang-format on */
    {
      return -1;
    }

  if ((dr->file_id_len % 2) == 0)
    {
      if (fseek (input_fptr, 1, SEEK_CUR) != 0)
        {
          fprintf (
              stderr,
              "ERROR: failed to seek past padding after directory record.\n");
          return -1;
        }
    }

  return 0;
}

/**
 *  Handle padding around sector boundaries.
 *  See: https://wiki.osdev.org/ISO_9660#Directories.
 */
static int
handle_sector_padding (FILE *input_fptr, const uint16_t lb_size)
{
  size_t pos = ftell (input_fptr);
  size_t next_sect = ((size_t)((pos + (lb_size - 1)) / lb_size)) * lb_size;
  if ((pos + sizeof (_ISO9660DirRec_t) - sizeof (char[UINT8_MAX])) > next_sect)
    {
      if (fseek (input_fptr, next_sect, SEEK_SET) != 0)
        {
          fprintf (stderr, "Failed to seek to next sector (%08lx).\n",
                   next_sect);
          return -1;
        }
    }

  return 0;
}

static int
resize_dr_list (_ISO9660DirRec_t **dr_list, size_t *list_cap)
{
  *list_cap *= 2;
  _ISO9660DirRec_t *tmp
      = realloc (*dr_list, *list_cap * sizeof (_ISO9660DirRec_t));
  if (tmp == NULL)
    return -1;

  *dr_list = tmp;
  return 0;
}

int
_i9660dr_dynamic_init (_ISO9660DirRec_t **dr_list, size_t list_cap[static 1],
                       size_t list_len[static 1], const uint16_t lb_size,
                       FILE input_fptr[static 1])
{
  if (dr_list == NULL)
    return -1;

  while (true)
    {
      handle_sector_padding (input_fptr, lb_size);

      _ISO9660DirRec_t curr = { 0 };
      if (_i9660dr_init (&curr, input_fptr) != 0)
        {
          fprintf (stderr, "Error reading directory record from file.\n");
          return -1;
        }

      if (curr.file_id_len == 0)
        break;

      if ((*list_len == *list_cap)
          && (resize_dr_list (dr_list, list_cap) != 0))
        {
          fprintf (stderr, "%s: Out of memory error.\n", __func__);
          return -1;
        }

      memcpy (&(*dr_list)[*list_len], &curr, sizeof (_ISO9660DirRec_t));
      (*list_len)++;
    }

  return 0;
}

/*
 *  Check for unsupported files.
 *
 *  Returns: True if file is unsupported, false otherwise.
 */
static bool
unsupported_file_check (uint8_t flags)
{
  const char err_msg[] = "Error extracting file using directory record";
  if (flags & (1 << (_I9660FF_NOT_FINAL_DIR_BIT)))
    {
      fprintf (stderr,
               "%s: Support for files split across multiple extents is not "
               "currently implemented.",
               err_msg);
      return true;
    }
  else if (flags & (1 << (_I9660FF_IS_ASSOC_FILE_BIT)))
    {
      fprintf (
          stderr,
          "%s: Support for associated files is not currently implemented.",
          err_msg);
      return true;
    }

  return false;
}

/*
 *  Warn about missing support for additional data.
 *  See: https://wiki.osdev.org/ISO_9660#Directories.
 */
static void
handle_missing_support_warnings (uint8_t flags)
{
  const char warning_fmt[] = "Warning: no implemented support for handling %s "
                             "stored in the extended attribute record.";

  if (flags & (1 << (_I9660FF_FMT_IN_EAR_BIT)))
    printf (warning_fmt, "additional format information");
  if (flags & (1 << (_I9660FF_PERMS_IN_EAR_BIT)))
    printf (warning_fmt, "owner and group permissions");
}

int
_i9660dr_extract (_ISO9660DirRec_t *dr, const uint16_t lb_size,
                  FILE input_fptr[static 1], const char path[static 1],
                  const size_t path_len)
{
  if ((dr == NULL) || (unsupported_file_check (dr->file_flags)))
    return -1;

  handle_missing_support_warnings (dr->file_flags);

  if (fseek (input_fptr, dr->extent_loc * lb_size, SEEK_SET) != 0)
    {
      // FIXME: really could use an standardized error handling interface.
      fprintf (stderr, "%s: fseek error (0x%08X).\n", __func__,
               dr->extent_loc * lb_size);
      return -1;
    }

  uint8_t *data = malloc (sizeof (uint8_t) * dr->extent_size);
  if (data == NULL)
    {
      fprintf (stderr, "%s: out of memory error.\n", __func__);
      return -1;
    }

  if (_i9660br_read_u8_array (input_fptr, data, dr->extent_size) != 0)
    {
      free (data);
      return -1;
    }

  char *file_path = calloc (path_len, sizeof (char));
  if (file_path == NULL)
    {
      fprintf (stderr, "Out of memory error: %s.", __func__);
      return -1;
    }

  strcpy (file_path, path);
  strncat (file_path, dr->file_id, dr->file_id_len - 2); // remove the ";1"

  printf ("Extracting file: %s\n", file_path);
  if (_u_export_data (data, dr->extent_size, file_path) != 0)
    {
      free (file_path);
      free (data);
      return -1;
    }

  free (file_path);
  free (data);
  return 0;
}

size_t
_i9660dr_size (void)
{
  return sizeof (_ISO9660DirRec_t);
}

_FileFlags_t
_i9660dr_get_flags (_ISO9660DirRec_t *dr)
{
  if (dr == NULL)
    return (_FileFlags_t)-1;

  return dr->file_flags;
}
