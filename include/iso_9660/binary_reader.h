#ifndef _ISO_9660_BINARY_DATA_READER_H_
#define _ISO_9660_BINARY_DATA_READER_H_

#include <stdint.h>
#include <stdio.h>

/** Returns: zero on success, non-zero on failure. */
int _br_read_u8 (FILE *fptr, uint8_t *output);

/** Returns: zero on success, non-zero on failure. */
int _br_read_str (FILE *fptr, char *output, size_t length);

/**
 *  Reads a little-endian followed by big-endian encoded unsigned 32-bit
 *  integer. Double checks that these values match because, why not?
 *  Returns: zero on success, non-zero on failure.
 *  See: https://wiki.osdev.org/ISO_9660#Numerical_formats
 */
int _br_read_le_be_u32 (FILE *fptr, uint32_t *output);

/**
 *  Reads a little endian 32-bit integer from file.
 *  Returns: zero on success, non-zero on failure.
 */
int _br_read_le_u32 (FILE *fptr, uint32_t *output);

/**
 *  Reads a big endian 32-bit integer from file.
 *  Returns: zero on success, non-zero on failure.
 */
int _br_read_be_u32 (FILE *fptr, uint32_t *output);

/**
 *  Reads a little-endian followed by big-endian encoded unsigned 16-bit
 *  integer. Double checks that these values match because, why not?
 *  Returns: zero on success, non-zero on failure.
 *  See: https://wiki.osdev.org/ISO_9660#Numerical_formats
 */
int _br_read_le_be_u16 (FILE *fptr, uint16_t *output);

/**
 *  Reads a little endian 16-bit integer from file.
 *  Returns: zero on success, non-zero on failure.
 */
int _br_read_le_u16 (FILE *fptr, uint16_t *output);

/**
 *  Reads a big endian 16-bit integer from file.
 *  Returns: zero on success, non-zero on failure.
 */
int _br_read_be_u16 (FILE *fptr, uint16_t *output);

/** Returns: zero on success, non-zero on failure. */
int _br_read_u8_array (FILE *fptr, uint8_t *output, size_t length);

#endif /* _ISO_9660_BINARY_DATA_READER_H_ */
