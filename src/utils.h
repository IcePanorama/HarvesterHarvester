#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>
#include <stdio.h>

uint32_t read_little_endian_data_uint32_t (FILE *fptr);
uint32_t read_both_endian_data_uint32 (FILE* fptr);
uint16_t read_both_endian_data_uint16 (FILE* fptr);
void read_string (FILE *fptr, char* output, uint8_t length);
void read_array_uint8 (FILE *fptr, uint8_t *arr, uint8_t length);

#endif
