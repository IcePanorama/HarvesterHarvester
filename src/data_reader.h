#ifndef _DATA_READER_H_
#define _DATA_READER_H_

#include "datetime.h"
#include "file_flags.h"

#include <stdint.h>
#include <stdio.h>

int8_t read_little_endian_data_uint32_t (FILE *fptr, uint32_t *output);

/**
 *  When data in a given file is presented first in little endian form and then
 *  repeated in big endian form, this function will read both values, double
 *  checking that the first matches the second. This is almost certainly
 *  unnecessary, but it makes me feel better :).
 *
 *  FIXME: incorrect, need to rewrite
 *  @param  fptr  pointer to data to be read.
 *  @return uint32_t containing the read data.
 *  @see read_both_endian_data_unint16()
 */
int8_t read_both_endian_data_uint32 (FILE *fptr, uint32_t *output);

int8_t read_little_endian_data_uint16_t (FILE *fptr, uint16_t *output);

/**
 *  When data in a given file is presented first in little endian form and then
 *  repeated in big endian form, this function will read both values, double
 *  checking that the first matches the second. This is almost certainly
 *  unnecessary, but it makes me feel better :).
 *
 *  @param  fptr  pointer to data to be read.
 *  @return uint16_t containing the read data.
 *  @see read_both_endian_data_unint32()
 */
uint16_t read_both_endian_data_uint16 (FILE *fptr);

void read_string (FILE *fptr, char *output, uint8_t length);
void read_array_uint8 (FILE *fptr, uint8_t *arr, uint8_t length);
int8_t read_dec_datetime (FILE *fptr, dec_datetime *dt);
int8_t read_single_uint8 (FILE *fptr, uint8_t *value);
int8_t read_dir_datetime (FILE *fptr, dir_datetime *dt);

/**
 *  Reads file flag data in from a given source pointed to by `fptr` and uses
 *  that data to update the various flags in `ff` as needed.
 *
 *  @param  fptr  Pointer to file with file flag data
 *  @param  ff    file flags to be updated.
 *  @see  file_flags
 */
int8_t read_file_flags (FILE *fptr, file_flags *ff);

#endif
