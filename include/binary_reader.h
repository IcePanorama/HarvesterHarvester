/**
 *  binary_reader.h - utilities for reading from binary files.
 *  All exposed functions are prefixed with the letters 'br'.
 */
#ifndef _BINARY_DATA_READER_H_
#define _BINARY_DATA_READER_H_

#include <stdint.h>
#include <stdio.h>

/** @returns zero on success, non-zero on failure. */
int8_t br_read_u8_from_file (FILE *fptr, uint8_t *output);

/** @returns zero on success, non-zero on failure. */
int8_t br_read_str_from_file (FILE *fptr, char *output, size_t length);

/**
 *  Reads a little-endian followed by big-endian encoded unsigned 32-bit
 *  integer. Double checks that these values match because, why not?
 *  @returns zero on success, non-zero on failure.
 *  @see https://wiki.osdev.org/ISO_9660#Numerical_formats
 */
int8_t br_read_le_be_u32_from_file (FILE *fptr, uint32_t *output);

/**
 *  Reads a little endian 32-bit integer from file.
 *  @returns zero on success, non-zero on failure.
 */
int8_t br_read_le_u32_from_file (FILE *fptr, uint32_t *output);

/**
 *  Reads a big endian 32-bit integer from file.
 *  @returns zero on success, non-zero on failure.
 */
int8_t br_read_be_u32_from_file (FILE *fptr, uint32_t *output);

/**
 *  Reads a little-endian followed by big-endian encoded unsigned 16-bit
 *  integer. Double checks that these values match because, why not?
 *  @returns zero on success, non-zero on failure.
 *  @see https://wiki.osdev.org/ISO_9660#Numerical_formats
 */
int8_t br_read_le_be_u16_from_file (FILE *fptr, uint16_t *output);

/**
 *  Reads a little endian 16-bit integer from file.
 *  @returns zero on success, non-zero on failure.
 */
int8_t br_read_le_u16_from_file (FILE *fptr, uint16_t *output);

/**
 *  Reads a big endian 16-bit integer from file.
 *  @returns zero on success, non-zero on failure.
 */
int8_t br_read_be_u16_from_file (FILE *fptr, uint16_t *output);

/** @returns zero on success, non-zero on failure. */
int8_t br_read_u8_array_from_file (FILE *fptr, uint8_t *output, size_t length);

#endif /* _BINARY_DATA_READER_H_ */
