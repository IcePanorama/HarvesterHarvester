#ifndef _UTILS_H_
#define _UTILS_H_

#include "file_flags.h"

#include <stdint.h>
#include <stdio.h>

// A lot of these probably shouldn't be in a utils.h file
// Should probably make a data.h or dat.h or something like that
uint32_t read_little_endian_data_uint32_t (FILE *fptr);
uint32_t read_both_endian_data_uint32 (FILE *fptr);
uint16_t read_little_endian_data_uint16_t (FILE *fptr);
uint16_t read_both_endian_data_uint16 (FILE *fptr);
void read_string (FILE *fptr, char *output, uint8_t length);
void read_array_uint8 (FILE *fptr, uint8_t *arr, uint8_t length);
struct dec_datetime read_dec_datetime (FILE *fptr);
uint8_t read_single_uint8 (FILE *fptr);
struct dir_datetime read_dir_datetime (FILE *fptr);
// these three are probably find though.
uint16_t change_endianness_uint16 (uint16_t value);
void read_file_flags (FILE *fptr, file_flags *ff);
void prepend_path_string (char *str, const char *prefix);

#endif
