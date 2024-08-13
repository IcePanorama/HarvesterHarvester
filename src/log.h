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
 *  log.h - wrapper for stdout functions.
 *
 *  As evidenced by the single function below, this file is rather
 *  underutilized at the moment. In the future this should host functions
 *  related to printing data out to log files, hence the name.
 */
#ifndef _HH_LOG_H_
#define _HH_LOG_H_

#include <stdint.h>
#include <stdio.h>

typedef enum hh_log_level
{
  HH_INFO,
  HH_WARNING,
  HH_ERROR,
} hh_log_level;

void hh_log (hh_log_level lvl, const char *fmt, ...);

/**
 *  Prints the next `BYTES_TO_READ` many bytes to the stdout, formatted in
 *  order to be easy to read. Useful for debugging. Note that `BYTES_TO_READ`
 *  is defined in `log.c`.
 */
void print_some_data_from_file (FILE *fptr);

#endif
