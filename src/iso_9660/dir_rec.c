#include "iso_9660/dir_rec.h"
#include "iso_9660/binary_reader.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int
_dr_init (_DirRec_t dr[static 1], FILE input_fptr[static 1])
{
  if ((_br_read_u8 (input_fptr, &dr->len) != 0)
      || (_br_read_u8 (input_fptr, &dr->extended_attrib_rec_len) != 0)
      || (_br_read_le_be_u32 (input_fptr, &dr->extent_loc) != 0)
      || (_br_read_le_be_u32 (input_fptr, &dr->extent_size) != 0)
      || (_br_read_u8 (input_fptr, &dr->recording_date_time.year) != 0)
      || (_br_read_u8 (input_fptr, &dr->recording_date_time.month) != 0)
      || (_br_read_u8 (input_fptr, &dr->recording_date_time.day) != 0)
      || (_br_read_u8 (input_fptr, &dr->recording_date_time.hour) != 0)
      || (_br_read_u8 (input_fptr, &dr->recording_date_time.minute) != 0)
      || (_br_read_u8 (input_fptr, &dr->recording_date_time.second) != 0)
      || (_br_read_u8 (input_fptr, &dr->recording_date_time.timezone) != 0)
      || (_br_read_u8 (input_fptr, &dr->file_flags) != 0)
      || (_br_read_u8 (input_fptr, &dr->file_unit_size) != 0)
      || (_br_read_u8 (input_fptr, &dr->interleave_gap_size) != 0)
      || (_br_read_le_be_u16 (input_fptr, &dr->vol_seq_num) != 0)
      || (_br_read_u8 (input_fptr, &dr->file_id_len) != 0)
      || (_br_read_str (input_fptr, dr->file_id, dr->file_id_len) != 0))
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

void
_dr_print (_DirRec_t dr[static 1])
{
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
  printf ("Hidden? %s\n",
          (dr->file_flags & (1 << (_FF_HIDDEN_FILE_BIT))) ? "true" : "false");
  printf ("Directory? %s\n",
          (dr->file_flags & (1 << (_FF_IS_DIRECTORY_BIT))) ? "true" : "false");
  printf ("Associated file? %s\n",
          (dr->file_flags & (1 << (_FF_IS_ASSOC_FILE_BIT))) ? "true"
                                                            : "false");
  printf ("Format info in extended attrib. record? %s\n",
          (dr->file_flags & (1 << (_FF_FMT_IN_EAR_BIT))) ? "true" : "false");
  printf ("Owner/group perms in extended attrib. record? %s\n",
          (dr->file_flags & (1 << (_FF_PERMS_IN_EAR_BIT))) ? "true" : "false");
  printf ("Not final directory record? %s\n",
          (dr->file_flags & (1 << (_FF_NOT_FINAL_DIR_BIT))) ? "true"
                                                            : "false");

  printf ("File unit size: %d\n", dr->file_unit_size);
  printf ("Interleave gap size: %d\n", dr->interleave_gap_size);
  printf ("Volume sequence number: %d\n", dr->vol_seq_num);
  printf ("File identifier length: %d\n", dr->file_id_len);
  printf ("File identifier: %.*s\n", dr->file_id_len, dr->file_id);
}

int
_dr_dynamic_init (_DirRec_t *dr_list[static 1], size_t list_cap[static 1],
                  size_t list_len[static 1], uint16_t lb_size,
                  FILE input_fptr[static 1])
{
  while (true)
    {
      /*
       *  Handle padding around sector boundaries.
       *  See: https://wiki.osdev.org/ISO_9660#Directories.
       */
      size_t pos = ftell (input_fptr);
      size_t next_sect = ((size_t)((pos + (lb_size - 1)) / lb_size)) * lb_size;
      if ((pos + sizeof (_DirRec_t) - sizeof (char[UINT8_MAX])) > next_sect)
        {
          if (fseek (input_fptr, next_sect, SEEK_SET) != 0)
            {
              fprintf (stderr, "Failed to seek to next sector (%08lx).\n",
                       next_sect);
              return -1;
            }
        }

      _DirRec_t curr = { 0 };
      if (_dr_init (&curr, input_fptr) != 0)
        {
          fprintf (stderr, "Error reading directory record from file.\n");
          return -1;
        }

      if (curr.file_id_len == 0)
        break;

      if (*list_len == *list_cap)
        {
          *list_cap *= 2;
          _DirRec_t *tmp = realloc (*dr_list, *list_cap * sizeof (_DirRec_t));
          if (tmp == NULL)
            {
              fprintf (stderr,
                       "Failed to grow directory record list to size %ld\n",
                       *list_cap);
              return -1;
            }
          *dr_list = tmp;
        }

      memcpy (&(*dr_list)[*list_len], &curr, sizeof (_DirRec_t));
      (*list_len)++;
    }

  return 0;
}

static int
create_export_dir (const char path[static 1])
{
  char *path_cpy = malloc (sizeof (char) * strlen (path) + 1);
  if (path_cpy == NULL)
    {
      fprintf (stderr, "%s: out of memory error.\n", __func__);
      return -1;
    }

  char *curr_path = calloc (strlen (path) + 1, sizeof (char));
  if (curr_path == NULL)
    {
      fprintf (stderr, "%s: out of memory error.\n", __func__);
      free (path_cpy);
      return -1;
    }

  strcpy (path_cpy, path);
  char *tok = strtok (path_cpy, "/");
  while (tok != NULL)
    {
      if (strchr (tok, '.') != NULL) // skip files.
        break;

      strcat (curr_path, tok);
      struct stat path_info;
      if (stat (curr_path, &path_info) != 0)
        {
          int status = mkdir (curr_path, 0700);
          if (status != 0)
            {
              fprintf (stderr, "Failed to create output directory: %s.\n",
                       curr_path);
              free (curr_path);
              free (path_cpy);
              return -1;
            }
        }

      tok = strtok (NULL, "/");
      if (tok != NULL)
        strcat (curr_path, "/");
    }
  free (curr_path);
  free (path_cpy);
  return 0;
}

static int
export_data (uint8_t data[static 1], size_t data_size,
             const char path[static 1])
{
  if (create_export_dir (path) != 0)
    return -1;

  FILE *output_file = fopen (path, "wb");
  if (output_file == NULL)
    {
      fprintf (stderr, "Failed to open file for export: %s.\n", path);
      return -1;
    }

  int status = 0;
  if (fwrite (data, sizeof (uint8_t), data_size, output_file) != data_size)
    {
      fprintf (stderr, "Error exporting file, %s.\n", path);
      status = -1; // still need to attempt `fclose` below.
    }

  if (fclose (output_file) != 0)
    {
      fprintf (stderr, "Error closing file, %s.\n", path);
      return -1;
    }

  return status;
}

/*
 *  Handle unsupported file errors.
 *  Returns: Zero if file is supported, non-zero otherwise.
 */
static int
unsupported_file_check (uint8_t flags)
{
  const char err_msg[] = "Error extracting file using directory record";
  if (flags & (1 << (_FF_NOT_FINAL_DIR_BIT)))
    {
      fprintf (stderr,
               "%s: Support for files split across multiple extents is not "
               "currently implemented.",
               err_msg);
      return -1;
    }
  else if (flags & (1 << (_FF_IS_ASSOC_FILE_BIT)))
    {
      fprintf (
          stderr,
          "%s: Support for associated files is not currently implemented.",
          err_msg);
      return -1;
    }

  return 0;
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
  if (flags & (1 << (_FF_FMT_IN_EAR_BIT)))
    printf (warning_fmt, "additional format information");
  if (flags & (1 << (_FF_PERMS_IN_EAR_BIT)))
    printf (warning_fmt, "owner and group permissions");
}

int
_dr_extract (_DirRec_t dr[static 1], size_t lb_size, FILE input_fptr[static 1],
             const char path[static 1], const size_t path_len)
{
  if (unsupported_file_check (dr->file_flags) != 0)
    return -1;

  handle_missing_support_warnings (dr->file_flags);

  if (fseek (input_fptr, dr->extent_loc * lb_size, SEEK_SET) != 0)
    {
      // FIXME: really could use an standardized error handling interface.
      fprintf (stderr, "%s: fseek error (0x%08lX).\n", __func__,
               dr->extent_loc * lb_size);
      return -1;
    }

  uint8_t *data = malloc (sizeof (uint8_t) * dr->extent_size);
  if (data == NULL)
    {
      fprintf (stderr, "%s: out of memory error.\n", __func__);
      return -1;
    }

  if (_br_read_u8_array (input_fptr, data, dr->extent_size) != 0)
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
  if (export_data (data, dr->extent_size, file_path) != 0)
    {
      free (file_path);
      free (data);
      return -1;
    }

  free (file_path);
  free (data);
  return 0;
}
