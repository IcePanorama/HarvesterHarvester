#include "iso_9660/dir_rec.h"
#include "iso_9660/binary_reader.h"

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
  printf ("Location of extent: 0x%08X\n", dr->extent_loc);
  printf ("Size of extent: %d\n", dr->extent_size);

  printf ("Recording date/time: %04d-%02d-%02d %02d:%02d:%02d (GMT%+02d)\n",
          1900 + dr->recording_date_time.year, dr->recording_date_time.month,
          dr->recording_date_time.day, dr->recording_date_time.hour,
          dr->recording_date_time.minute, dr->recording_date_time.second,
          (-48 + dr->recording_date_time.timezone) >> 2);

  puts ("File flags:");
  printf ("Hidden? %s\n", (dr->file_flags & 1) ? "true" : "false");
  printf ("Directory? %s\n", (dr->file_flags & 2) ? "true" : "false");
  printf ("Associated file? %s\n", (dr->file_flags & 4) ? "true" : "false");
  printf ("Format info in extended attrib. record? %s\n",
          (dr->file_flags & 8) ? "true" : "false");
  printf ("Owner/group perms in extended attrib. record? %s\n",
          (dr->file_flags & 16) ? "true" : "false");
  printf ("Not final directory record? %s\n",
          (dr->file_flags & 128) ? "true" : "false");

  printf ("File unit size: %d\n", dr->file_unit_size);
  printf ("Interleave gap size: %d\n", dr->interleave_gap_size);
  printf ("Volume sequence number: %d\n", dr->vol_seq_num);
  printf ("File identifier length: %d\n", dr->file_id_len);
  printf ("File identifier: %.*s\n", dr->file_id_len, dr->file_id);
}
