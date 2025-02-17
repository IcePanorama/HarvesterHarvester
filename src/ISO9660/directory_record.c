#include "ISO9660/directory_record.h"
#include "binary_reader.h"

#include <stdio.h>

static void print_file_flags (uint8_t file_flags);

int
read_dir_rec_from_file (FILE *fptr, ISO9660DirectoryRecord_t *dr)
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
print_dir_rec (ISO9660DirectoryRecord_t *dr)
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
