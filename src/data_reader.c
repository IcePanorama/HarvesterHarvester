#include "data_reader.h"
#include "errors.h"
#include "file_flags.h"

#include <stdbool.h>
#include <stdint.h>

static uint32_t read_big_endian_data_uint32_t (FILE *fptr);
static uint16_t read_big_endian_data_uint16_t (FILE *fptr);

int8_t
read_both_endian_data_uint32 (FILE *fptr, uint32_t *output)
{
  uint32_t value;
  if (read_little_endian_data_uint32_t (fptr, &value) != 0)
    return HH_FREAD_ERROR;

  uint32_t expected_value = read_big_endian_data_uint32_t (fptr);

  if (value != expected_value)
    {
      printf ("ERROR: Incorrect endian conversion (uint32_t).\n\tExpected "
              "%08X, got %08X.\n",
              expected_value, value);
    }

  *output = value;
  return 0;
}

uint16_t
read_both_endian_data_uint16 (FILE *fptr)
{
  uint16_t value;
  read_little_endian_data_uint16_t (fptr, &value);
  uint16_t expected_value = read_big_endian_data_uint16_t (fptr);

  if (value != expected_value)
    {
      printf ("ERROR: Incorrect endian conversion (uint16_t).\n\tExpected "
              "%04X, got %04X.\n",
              expected_value, value);
    }

  return value;
}

int8_t
read_little_endian_data_uint32_t (FILE *fptr, uint32_t *output)
{
  uint8_t bytes[4];
  size_t bytes_read = fread (bytes, sizeof (uint8_t), 4, fptr);
  if (bytes_read != 4)
    {
      handle_fread_error (bytes_read, sizeof (bytes));

      return HH_FREAD_ERROR;
    }

  *output = ((uint32_t)bytes[3] << 24) | ((uint32_t)bytes[2] << 16)
            | ((uint32_t)bytes[1] << 8) | (uint32_t)bytes[0];

  return 0;
}

uint32_t
read_big_endian_data_uint32_t (FILE *fptr)
{
  uint8_t bytes[4];
  size_t bytes_read = fread (bytes, sizeof (uint8_t), 4, fptr);
  if (bytes_read != 4)
    {
      handle_fread_error (bytes_read, sizeof (bytes));
    }

  return ((uint32_t)bytes[0] << 24) | ((uint32_t)bytes[1] << 16)
         | ((uint32_t)bytes[2] << 8) | (uint32_t)bytes[3];
}

int8_t
read_little_endian_data_uint16_t (FILE *fptr, uint16_t *output)
{
  uint8_t bytes[2];
  size_t bytes_read = fread (bytes, sizeof (uint8_t), 2, fptr);
  if (bytes_read != 2)
    {
      handle_fread_error (bytes_read, sizeof (bytes));
      return HH_FREAD_ERROR;
    }

  *output = ((uint16_t)bytes[0] << 8) | (uint16_t)bytes[1];
  return 0;
}

uint16_t
read_big_endian_data_uint16_t (FILE *fptr)
{
  uint8_t bytes[2];
  size_t bytes_read = fread (bytes, sizeof (uint8_t), 2, fptr);
  if (bytes_read != 2)
    {
      handle_fread_error (bytes_read, sizeof (bytes));
    }
  return ((uint16_t)bytes[1] << 8) | (uint16_t)bytes[0];
}

void
read_string (FILE *fptr, char *output, uint8_t length)
{
  size_t bytes_read = fread (output, sizeof (char), length - 1, fptr);
  output[length - 1] = '\0';
  if (bytes_read != sizeof (char) * length - 1)
    {
      handle_fread_error (bytes_read, sizeof (char) * length - 1);
    }
}

void
read_array_uint8 (FILE *fptr, uint8_t *arr, uint8_t length)
{
  size_t bytes_read = fread (arr, sizeof (uint8_t), length, fptr);
  if (bytes_read != sizeof (uint8_t) * length)
    {
      handle_fread_error (bytes_read, sizeof (uint8_t) * length);
    }
}

int8_t
read_dec_datetime (FILE *fptr, dec_datetime *dt)
{
  read_string (fptr, dt->year, YEAR_FIELD_LEN);
  read_string (fptr, dt->month, MONTH_FIELD_LEN);
  read_string (fptr, dt->day, DAY_FIELD_LEN);
  read_string (fptr, dt->hour, HOUR_FIELD_LEN);
  read_string (fptr, dt->minute, MINUTE_FIELD_LEN);
  read_string (fptr, dt->second, SECOND_FIELD_LEN);
  read_string (fptr, dt->hundredths_of_a_second,
               HUNDREDTHS_OF_A_SECOND_FIELD_LEN);

  if (read_single_uint8 (fptr, &dt->time_zone_offset) != 0)
    return HH_FREAD_ERROR;

  return 0;
}

int8_t
read_single_uint8 (FILE *fptr, uint8_t *value)
{
  size_t bytes_read = fread (value, sizeof (uint8_t), 1, fptr);
  if (bytes_read != sizeof (uint8_t))
    {
      handle_fread_error (bytes_read, sizeof (uint8_t));
      return HH_FREAD_ERROR;
    }

  return 0;
}

int8_t
read_dir_datetime (FILE *fptr, dir_datetime *dt)
{
  if ((read_single_uint8 (fptr, &dt->year) != 0)
      || (read_single_uint8 (fptr, &dt->month) != 0)
      || (read_single_uint8 (fptr, &dt->day) != 0)
      || (read_single_uint8 (fptr, &dt->hour) != 0)
      || (read_single_uint8 (fptr, &dt->minute) != 0)
      || (read_single_uint8 (fptr, &dt->second) != 0)
      || (read_single_uint8 (fptr, &dt->time_zone_offset) != 0))
    {
      return HH_FREAD_ERROR;
    }

  return 0;
}

int8_t
read_file_flags (FILE *fptr, file_flags *ff)
{
  uint8_t byte;
  if (read_single_uint8 (fptr, &byte) != 0)
    return HH_FREAD_ERROR;

  if (byte & 0x1)
    {
      ff->hidden = true;
    }
  if (byte & 0x2)
    {
      ff->subdirectory = true;
    }
  if (byte & 0x4)
    {
      ff->associated_file = true;
    }
  if (byte & 0x8)
    {
      ff->extended_attribute_record_contains_owner_and_group_permissions
          = true;
    }
  if (byte & 0x10)
    {
      ff->final_directory_record = true;
    }

  return 0;
}
