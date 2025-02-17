#include "ISO9660/directory_record.h"
#include "binary_reader.h"

#include <stdio.h>

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
