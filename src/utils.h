#ifndef _UTILS_H_
#define _UTILS_H_

#include "file_flags.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

uint16_t change_endianness_uint16 (uint16_t value);
void read_file_flags (FILE *fptr, file_flags *ff);
void prepend_path_string (char *str, const char *prefix);
bool is_string_dat_file (char *str);

#endif
