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
/** output.h - container for functions related to the output directory. */
#ifndef _HH_OUTPUT_H_
#define _HH_OUTPUT_H_

#include <stdbool.h>
#include <stdint.h>

/**
 *  Creates an output directory at the given path if it doesn't already exist.
 *
 *  @return zero on success, non-zero on failure.
 */
// TODO: path should be of type `const char *`
int8_t create_output_directory (char *path);

/** TODO: add documentaion. */
int8_t create_new_output_directory (const char *path);

#endif
