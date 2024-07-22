#ifndef _DEC_DATETIME_H_
#define _DEC_DATETIME_H_

#include <stdint.h>

#define YEAR_FIELD_LEN 5
#define MONTH_FIELD_LEN 3
#define DAY_FIELD_LEN 3
#define HOUR_FIELD_LEN 3
#define MINUTE_FIELD_LEN 3
#define SECOND_FIELD_LEN 3
#define HUNDREDTHS_OF_A_SECOND_FIELD_LEN 3

/**
 * The date/time format used in the Primary Volume Descriptor. Uses ASCII
 * digits to represent the main parts of the date/time.
 *
 * @see https://wiki.osdev.org/ISO_9660#Date/time_format
 * @see volume_descriptor
 */
typedef struct dec_datetime
{
  char year[5];
  char month[3];
  char day[3];
  char hour[3];
  char minute[3];
  char second[3];
  char hundredths_of_a_second[3];
  uint8_t time_zone_offset;
} dec_datetime;

/**
 * The date/time format used by directory records. This format is presumably
 * used to save disc space over a large number of entries.
 *
 * @see https://wiki.osdev.org/ISO_9660#Directories
 * @see directory_record
 */
typedef struct dir_datetime
{
  uint8_t year; //!< Number of years since 1900.
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t time_zone_offset;
} dir_datetime;

/**
 * Prints the value of a `dec_datetime` to the console in a human-readable
 * form.
 *
 * @see dec_datetime
 */
void print_dec_datetime (dec_datetime dt);

/**
 * Prints the value of a `dir_datetime` to the console in a human-readable
 * form.
 *
 * @see dir_datetime
 */
void print_dir_datetime (dir_datetime dt);

#endif
