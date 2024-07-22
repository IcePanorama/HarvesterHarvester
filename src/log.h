#ifndef _LOG_H_
#define _LOG_H_

#include <stdint.h>
#include <stdio.h>

/**
 *  Prints out the next `BYTES_TO_READ` many bytes to the stdout, formatted
 *  properly in order to be easy to read. Useful for debugging.  Note that
 *  `BYTES_TO_READ` is defined in log.c.
 *
 *  @param  fptr  pointer to where user currently is in a file
 */
void print_some_data_from_file (FILE *fptr);

#endif
