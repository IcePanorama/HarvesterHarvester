//  Copyright (C) 2024  IcePanorama
//  This file is a part of HarvesterHarvester.
//  HarvesterHarvester is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by the
//  Free Software Foundation, either version 3 of the License, or (at your
//  option) any later version.
//  This program is distributed in the hope that it will be useful, but WITHOUT
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//  more details.
//
//  You should have received a copy of the GNU General Public License along
//  with this program.  If not, see <https://www.gnu.org/licenses/>.
#include "directory.h"
#include "data_reader.h"
#include "errors.h"
#include "log.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static const size_t DIR_STARTING_NUM_RECORDS = 10;
static const size_t DIR_RECORDS_GROWTH_RATE = 2;

/**
 *  Expands the size of a given directory's `records` attribute by a factor of
 *  `DIR_RECORDS_GROWTH_RATE`.
 */
static int8_t resize_directory_records (directory *d);

int8_t
create_directory (directory *d)
{
  d->records = malloc (sizeof (directory_record) * DIR_STARTING_NUM_RECORDS);
  if (d->records == NULL)
    {
      hh_log (HH_LOG_ERROR,
              "Failed to allocate memory for directory records.");
      return HH_MEM_ALLOC_ERROR;
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

int8_t
add_record_to_directory (directory *d, directory_record *r)
{
  if (d->current_record >= d->size)
    {
      if (resize_directory_records (d) != 0)
        return -1;
    }

  d->records[d->current_record] = *r;
  d->current_record++;
  return 0;
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

int8_t
resize_directory_records (directory *d)
{
  size_t new_size = d->size * DIR_RECORDS_GROWTH_RATE;
  directory_record *new_records = (directory_record *)realloc (
      d->records, sizeof (directory_record) * new_size);
  if (new_records == NULL)
    {
      destroy_directory (d);
      return handle_realloc_error ("records", d->size, new_size);
    }

  d->records = new_records;
  for (size_t i = d->size; i < new_size; i++)
    {
      d->records[i].file_identifier = NULL;
    }

  d->size = new_size;
  return 0;
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

      if ((read_single_uint8 (fptr, &dr.extended_attribute_record_length) != 0)
          || (read_both_endian_data_uint32 (fptr, &dr.location_of_extent) != 0)
          || (read_both_endian_data_uint32 (fptr, &dr.data_length) != 0)
          || (read_dir_datetime (fptr, &dr.recording_datetime) != 0))
        {
          return HH_FREAD_ERROR;
        }

      dr.file_flags = create_file_flags ();
      read_file_flags (fptr, &dr.file_flags);

      if (read_single_uint8 (fptr, &dr.file_unit_size) != 0
          || read_single_uint8 (fptr, &dr.interleave_gap_size) != 0)
        {
          return HH_FREAD_ERROR;
        }

      if (read_both_endian_data_uint16 (fptr, &dr.volume_sequence_number) != 0)
        return HH_FREAD_ERROR;

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

      if (add_record_to_directory (d, &dr) != 0)
        {
          return HH_MEM_ALLOC_ERROR;
        }

      if (dr.file_identifier_length % 2 != 0) // handle padding field
        fseek (fptr, 1, SEEK_CUR);

      if (read_single_uint8 (fptr, &single_byte) != 0)
        return HH_FREAD_ERROR;
    }
  while (single_byte != 0);

  return 0;
}
