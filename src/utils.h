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

int8_t prepend_path_string (char *str, const char *prefix);
bool is_string_dat_file (char *str);
bool directory_exists (const char *dir);
bool file_exists (const char *filename);
bool peek_char_is (FILE *fptr, uint8_t expected_ch);
bool peek_eof (FILE *fptr);
void prepend_string (char *str, const char *prefix);

#endif
