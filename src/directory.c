#include "directory.h"
#include "errors.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

void resize_directory_records (directory *d);

static const size_t DIR_STARTING_NUM_RECORDS = 10;
static const size_t DIR_RECORDS_GROWTH_RATE = 2;

int8_t
create_directory (directory *d)
{
  d->records = malloc (sizeof (directory_record) * DIR_STARTING_NUM_RECORDS);
  if (d->records == NULL)
    {
      perror ("ERROR: Failed to allocate memory for directory.");
      return -1;
    }

  d->size = DIR_STARTING_NUM_RECORDS;
  d->current_record = 0;

  for (size_t i = 0; i < DIR_STARTING_NUM_RECORDS; i++)
    {
      d->records[i].file_identifier = NULL;
    }

  return 0;
}

void
destroy_directory (directory *d)
{
  for (size_t i = 0; i < d->current_record; i++)
    {
      if (d->records[i].file_identifier == NULL)
        continue;

      free (d->records[i].file_identifier);
      d->records[i].file_identifier = NULL;
    }

  free (d->records);
  d->records = NULL;
}

void
add_record_to_directory (directory *d, directory_record *r)
{
  if (d->current_record >= d->size)
    {
      resize_directory_records (d);
    }

  d->records[d->current_record] = *r;
  d->current_record++;
}

void
print_directory_record (directory_record *r)
{
  printf ("Record length: %02X\n", r->record_length);
  printf ("Extended attribute record length: %02X\n",
          r->extended_attribute_record_length);
  printf ("Location of extent: %08X\n", r->location_of_extent);
  printf ("Data length: %08X\n", r->data_length);
  print_dir_datetime (r->recording_datetime);
  print_file_flags (&r->file_flags);
  printf ("File unit size: %02X\n", r->file_unit_size);
  printf ("Interleave gap size: %02X\n", r->interleave_gap_size);
  printf ("Volume sequence number: %04X\n", r->volume_sequence_number);
  printf ("File identifier length: %02X\n", r->file_identifier_length);
  printf ("File identifier: %s\n", r->file_identifier);
}

void
resize_directory_records (directory *d)
{
  size_t new_size = d->size * DIR_RECORDS_GROWTH_RATE;
  directory_record *new_records = (directory_record *)realloc (
      d->records, sizeof (directory_record) * new_size);
  if (new_records == NULL)
    {
      perror ("ERROR: failed to reallocate memory for the directory records.");
      destroy_directory (d);
      exit (1);
    }

  d->records = new_records;
  for (size_t i = d->size; i < new_size; i++)
    {
      d->records[i].file_identifier = NULL;
    }

  d->size = new_size;
}

void
print_directory (directory *d)
{
  printf ("Number of records: %ld\n", d->current_record + 1);
  for (size_t i = 0; i < d->current_record; i++)
    {
      printf ("!!! Record #%ld\n", i + 1);
      print_directory_record (&d->records[i]);
      puts ("-------------------------------");
    }
}

/*
 *  process_directory
 *
 *  TODO: write some better documentation.
 *  In the meantime, see: https://wiki.osdev.org/ISO_9660#Directories
 */
void
process_directory (FILE *fptr, directory *d)
{
  uint8_t single_byte = read_single_uint8 (fptr);
  do
    {
      directory_record dr;
      dr.record_length = single_byte;
      dr.extended_attribute_record_length = read_single_uint8 (fptr);
      dr.location_of_extent = read_both_endian_data_uint32 (fptr);
      dr.data_length = read_both_endian_data_uint32 (fptr);
      dr.recording_datetime = read_dir_datetime (fptr);
      dr.file_flags = create_file_flags ();
      read_file_flags (fptr, &dr.file_flags);
      dr.file_unit_size = read_single_uint8 (fptr);
      dr.interleave_gap_size = read_single_uint8 (fptr);
      dr.volume_sequence_number = read_both_endian_data_uint16 (fptr);
      dr.file_identifier_length = read_single_uint8 (fptr) + 1;

      dr.file_identifier
          = (char *)calloc (dr.file_identifier_length, sizeof (char));
      size_t bytes_read = fread (dr.file_identifier, sizeof (char),
                                 dr.file_identifier_length - 1, fptr);
      dr.file_identifier[dr.file_identifier_length - 1] = '\0';
      if (bytes_read != sizeof (char) * (dr.file_identifier_length - 1))
        {
          handle_fread_error (fptr, bytes_read,
                              sizeof (char) * (dr.file_identifier_length - 1));
        }

      add_record_to_directory (d, &dr);

      if (dr.file_identifier_length % 2 != 0) // handle padding field
        fseek (fptr, 1, SEEK_CUR);

      single_byte = read_single_uint8 (fptr);
    }
  while (single_byte != 0);
}
