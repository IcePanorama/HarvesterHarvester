#include "directory.h"
#include "data_reader.h"
#include "errors.h"
#include "utils.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static void resize_directory_records (directory *d);

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
  printf ("Number of records: %zu\n", d->current_record + 1);
  for (size_t i = 0; i < d->current_record; i++)
    {
      printf ("!!! Record #%zu\n", i + 1);
      print_directory_record (&d->records[i]);
      puts ("-------------------------------");
    }
}

int8_t
process_directory (FILE *fptr, directory *d)
{
  uint8_t single_byte;
  if (read_single_uint8 (fptr, &single_byte) != 0)
    return HH_FREAD_ERROR;

  do
    {
      directory_record dr;
      dr.record_length = single_byte;

      if (read_single_uint8 (fptr, &dr.extended_attribute_record_length) != 0)
        return HH_FREAD_ERROR;

      dr.location_of_extent = read_both_endian_data_uint32 (fptr);
      dr.data_length = read_both_endian_data_uint32 (fptr);

      if (read_dir_datetime (fptr, &dr.recording_datetime) != 0)
        return HH_FREAD_ERROR;

      dr.file_flags = create_file_flags ();
      read_file_flags (fptr, &dr.file_flags);

      if (read_single_uint8 (fptr, &dr.file_unit_size) != 0
          || read_single_uint8 (fptr, &dr.interleave_gap_size) != 0)
        {
          return HH_FREAD_ERROR;
        }

      dr.volume_sequence_number = read_both_endian_data_uint16 (fptr);

      if (read_single_uint8 (fptr, &dr.file_identifier_length) != 0)
        {
          return HH_FREAD_ERROR;
        }
      dr.file_identifier_length++;

      dr.file_identifier
          = (char *)calloc (dr.file_identifier_length, sizeof (char));
      size_t bytes_read = fread (dr.file_identifier, sizeof (char),
                                 dr.file_identifier_length - 1, fptr);
      dr.file_identifier[dr.file_identifier_length - 1] = '\0';
      if (bytes_read != sizeof (char) * (dr.file_identifier_length - 1))
        {
          handle_fread_error (bytes_read,
                              sizeof (char) * (dr.file_identifier_length - 1));
          return HH_FREAD_ERROR;
        }

      add_record_to_directory (d, &dr);

      if (dr.file_identifier_length % 2 != 0) // handle padding field
        fseek (fptr, 1, SEEK_CUR);

      if (read_single_uint8 (fptr, &single_byte) != 0)
        return HH_FREAD_ERROR;
    }
  while (single_byte != 0);

  return 0;
}
