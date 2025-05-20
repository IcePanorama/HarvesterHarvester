#ifndef _HARVESTER_HARVESTER_BINARY_READER_H_
#define _HARVESTER_HARVESTER_BINARY_READER_H_

#include <stdint.h>
#include <stdio.h>

/** Returns: zero on success, non-zero on failure. */
int _hhbr_read_str (FILE fptr[static 1], char output[static 1], size_t length);

/**
 *  Reads a little endian 32-bit integer from file.
 *  Returns: zero on success, non-zero on failure.
 */
int _hhbr_read_le_u32 (FILE fptr[static 1], uint32_t output[static 1]);

/** Returns: zero on success, non-zero on failure. */
int _hhbr_read_u8_array (FILE *fptr, uint8_t *output, size_t length);

#endif /* _HARVESTER_HARVESTER_BINARY_READER_H_ */
