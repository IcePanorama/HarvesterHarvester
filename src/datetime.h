/**
 *  datetime.h - contains the `dec_datetime` and `dir_datetime` data types and
 *  their associated functions.
 */
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
#ifndef _HH_DATETIME_H_
#define _HH_DATETIME_H_

#include <stdint.h>

#define YEAR_FIELD_LEN 5
#define MONTH_FIELD_LEN 3
#define DAY_FIELD_LEN 3
#define HOUR_FIELD_LEN 3
#define MINUTE_FIELD_LEN 3
#define SECOND_FIELD_LEN 3
#define HUNDREDTHS_OF_A_SECOND_FIELD_LEN 3

/**
 * The date/time format used by Primary Volume Descriptors. Uses ASCII
 * digits to represent the main parts of the date/time.
 *
 * @see https://wiki.osdev.org/ISO_9660#Date/time_format
 * @see volume_descriptor
 */
typedef struct dec_datetime
{
  char year[YEAR_FIELD_LEN];
  char month[MONTH_FIELD_LEN];
  char day[HOUR_FIELD_LEN];
  char hour[HOUR_FIELD_LEN];
  char minute[MINUTE_FIELD_LEN];
  char second[SECOND_FIELD_LEN];
  char hundredths_of_a_second[HUNDREDTHS_OF_A_SECOND_FIELD_LEN];
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
 * Prints the attributes of a `dec_datetime` to the console in a human-readable
 * form.
 *
 * @see dec_datetime
 */
void print_dec_datetime (dec_datetime dt);

/**
 * Prints the attributes of a `dir_datetime` to the console in a human-readable
 * form.
 *
 * @see dir_datetime
 */
void print_dir_datetime (dir_datetime dt);

#endif
