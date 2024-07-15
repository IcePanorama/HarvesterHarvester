/* clang-format off */
#include <stdio.h>
#include <stdlib.h>

#include "directory.h"
/* clang-format on */

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
