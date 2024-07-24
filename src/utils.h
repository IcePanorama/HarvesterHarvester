#ifndef _UTILS_H_
#define _UTILS_H_

#include "file_flags.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/**
 *  Swaps a given `uint16_t` from little endian to big endian or vice-versa.
 *
 *  @param  value uint16_t to be swapped.
 *  @return value with its endianness swapped.
 */
uint16_t change_endianness_uint16 (uint16_t value);

/**
 *  Reads file flag data in from a given source pointed to by `fptr` and uses
 *  that data to update the various flags in `ff` as needed.
 *
 *  @param  fptr  Pointer to file with file flag data
 *  @param  ff    file flags to be updated.
 *  @see  file_flags
 */
void read_file_flags (FILE *fptr, file_flags *ff);
void prepend_path_string (char *str, const char *prefix);
bool is_string_dat_file (char *str);
bool directory_exists (const char *dir);

#endif
