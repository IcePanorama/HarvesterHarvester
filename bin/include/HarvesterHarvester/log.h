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
/**
 *  log.h - wrapper for hh_log and its related functions functions.
 */
#ifndef _HH_LOG_H_
#define _HH_LOG_H_

#include <stdint.h>
#include <stdio.h>

/**
 *  The three levels of log messages: INFO, WARNING, and ERROR. When used with
 *  the `hh_log()` function, these log_levels modify the output string and
 *  stream.
 *
 *  @see hh_log
 */
typedef enum hh_log_level
{
  HH_LOG_INFO,
  HH_LOG_WARNING,
  HH_LOG_ERROR,
} hh_log_level;

/**
 *  A logger function. Outputs fmt and args to either the stdout or stderr
 *  depending on the `lvl` passed into the function.
 */
void hh_log (hh_log_level lvl, const char *fmt, ...);

/**
 *  Prints the next `BYTES_TO_READ` many bytes to the stdout, formatted in
 *  order to be easy to read. Useful for debugging. Note that `BYTES_TO_READ`
 *  is defined in `log.c`.
 */
void print_some_data_from_file (FILE *fptr);

#endif
