#include "iso_9660/directory_record.h"
#include "binary_reader.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// FIXME: this is redefined in multiple different files, should be a common def
#define LIST_DIR_RECORD_STARTING_NUM_ENTRIES (10)

static void print_file_flags (uint8_t file_flags);
/*
 *  When processing a list of directory records: "even if a directory spans
 *  multiple sectors, the directory entries are not permitted to cross the
 * sector boundary ... where there is not enough space to record an entire
 * directory entry at the end of a sector, that sector is zero-padded and the
 * next consecutive sector is used."
 *  @param  fptr  the file to seek through.
 *  @param  lbs   the logical block size of the given file's filesystem.
 *  @see: https://wiki.osdev.org/ISO_9660#Directories
 */
static int handle_sector_boundary_padding (FILE *fptr, uint16_t lbs);

int
read_dir_rec_from_file (FILE fptr[static 1],
                        ISO9660DirectoryRecord_t dr[static 1])
{
  if ((br_read_u8_from_file (fptr, &dr->dir_rec_length) != 0)
      || (br_read_u8_from_file (fptr, &dr->extended_attrib_rec_length) != 0)
      || (br_read_le_be_u32_from_file (fptr, &dr->extent_location) != 0)
      || (br_read_le_be_u32_from_file (fptr, &dr->extent_size) != 0)
      || (br_read_u8_from_file (fptr, &dr->recording_date_time.year) != 0)
      || (br_read_u8_from_file (fptr, &dr->recording_date_time.month) != 0)
      || (br_read_u8_from_file (fptr, &dr->recording_date_time.day) != 0)
      || (br_read_u8_from_file (fptr, &dr->recording_date_time.hour) != 0)
      || (br_read_u8_from_file (fptr, &dr->recording_date_time.minute) != 0)
      || (br_read_u8_from_file (fptr, &dr->recording_date_time.second) != 0)
      || (br_read_u8_from_file (fptr, &dr->recording_date_time.timezone) != 0)
      || (br_read_u8_from_file (fptr, &dr->file_flags) != 0)
      || (br_read_u8_from_file (fptr, &dr->file_unit_size) != 0)
      || (br_read_u8_from_file (fptr, &dr->interleave_gap_size) != 0)
      || (br_read_le_be_u16_from_file (fptr, &dr->volume_sequence_number) != 0)
      || (br_read_u8_from_file (fptr, &dr->file_identifier_length) != 0)
      || (br_read_str_from_file (fptr, dr->file_identifier,
                                 dr->file_identifier_length)
          != 0))
    {
      return -1;
    }

  if ((dr->file_identifier_length % 2) == 0)
    {
      if (fseek (fptr, 1, SEEK_CUR) != 0)
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
print_dir_rec (ISO9660DirectoryRecord_t dr[static 1])
{
  printf ("-- Directory record length: %d\n", dr->dir_rec_length);
  printf ("-- Extended attribute length: %d\n",
          dr->extended_attrib_rec_length);
  printf ("-- Location of extent: 0x%08X\n", dr->extent_location);
  printf ("-- Size of extent: %d\n", dr->extent_size);

  printf ("-- Recording date/time: %04d-%02d-%02d %02d:%02d:%02d (GMT%+02d)\n",
          1900 + dr->recording_date_time.year, dr->recording_date_time.month,
          dr->recording_date_time.day, dr->recording_date_time.hour,
          dr->recording_date_time.minute, dr->recording_date_time.second,
          (-48 + dr->recording_date_time.timezone) >> 2);

  puts ("-- File flags:");
  print_file_flags (dr->file_flags);

  printf ("-- File unit size: %d\n", dr->file_unit_size);
  printf ("-- Interleave gap size: %d\n", dr->interleave_gap_size);
  printf ("-- Volume sequence number: %d\n", dr->volume_sequence_number);
  printf ("-- File identifier length: %d\n", dr->file_identifier_length);
  printf ("-- File identifier: %.*s\n", dr->file_identifier_length,
          dr->file_identifier);
}

void
print_file_flags (uint8_t file_flags)
{
  printf ("--- Hidden? %s\n", (file_flags & 1) ? "true" : "false");
  printf ("--- Directory? %s\n", (file_flags & 2) ? "true" : "false");
  printf ("--- Associated file? %s\n", (file_flags & 4) ? "true" : "false");
  printf ("--- Format info in extended attrib. record? %s\n",
          (file_flags & 8) ? "true" : "false");
  printf ("--- Owner/group perms in extended attrib. record? %s\n",
          (file_flags & 16) ? "true" : "false");
  printf ("--- Not final directory record? %s\n",
          (file_flags & 128) ? "true" : "false");
}

int
populate_directory_record_list (FILE input_fptr[static 1], uint16_t lbs,
                                ISO9660PathTableEntry_t pt_list[static 1],
                                size_t pt_list_len,
                                ISO9660DirectoryRecord_t *dr_list[static 1],
                                size_t dr_list_lens[static 1])
{
  size_t i, j;

  for (i = 0; i < pt_list_len; i++)
    {
      dr_list_lens[i] = (LIST_DIR_RECORD_STARTING_NUM_ENTRIES);

      dr_list[i] = calloc (dr_list_lens[i], sizeof (ISO9660DirectoryRecord_t));
      if (dr_list[i] == NULL)
        {
          fprintf (
              stderr,
              "ERROR: Failed to allocate memory for directory record list.");
          goto clean_up;
        }

      if (fseek (input_fptr, pt_list[i].extent_location * lbs, SEEK_SET) != 0)
        {
          fprintf (
              stderr,
              "ERROR: Failed to seek to directory record location (0x%08X).\n",
              pt_list[i].extent_location * lbs);
          goto clean_up;
        }

      j = 0;
      while (1)
        {
          if (handle_sector_boundary_padding (input_fptr, lbs) != 0)
            goto clean_up;

          ISO9660DirectoryRecord_t dir;
          if (read_dir_rec_from_file (input_fptr, &dir) != 0)
            goto clean_up;

          if (dir.file_identifier_length == 0)
            break;

          if (j >= dr_list_lens[i])
            {
              dr_list_lens[i] *= 2;
              ISO9660DirectoryRecord_t *temp = realloc (
                  dr_list[i],
                  dr_list_lens[i] * sizeof (ISO9660DirectoryRecord_t));
              if (temp == NULL)
                {
                  fprintf (stderr, "ERROR: Failed to reallocate memory for "
                                   "directory record.");
                  goto realloc_failure;
                }
              dr_list[i] = temp;
            }

          memcpy ((void *)&dr_list[i][j], &dir,
                  sizeof (ISO9660DirectoryRecord_t));
          j++;
        }

      dr_list_lens[i] = j;
    }

  return 0;
realloc_failure:
  u_free_partial_list_elements ((void **)&dr_list[i], 0, j);
clean_up:
  u_free_partial_list_elements ((void **)dr_list, 0, i);
  return -1;
}

int
handle_sector_boundary_padding (FILE *fptr, uint16_t lbs)
{
  size_t curr_sector = (size_t)(ftell (fptr) / lbs);
  size_t next_sector
      = (size_t)((ftell (fptr) + sizeof (ISO9660DirectoryRecord_t)) / lbs);

  if (next_sector != curr_sector) // Entry crosses sector boundary
    {
      if (fseek (fptr, (size_t)((ftell (fptr) + (lbs - 1)) / lbs), SEEK_SET)
          != 0)
        {
          fprintf (stderr, "ERROR: Failed to seek across sector boundary.\n");
          return -1;
        }
    }

  return 0;
}
