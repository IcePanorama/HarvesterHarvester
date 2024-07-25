#ifndef _ERRORS_H_
#define _ERRORS_H_

#include <stdint.h>
#include <stdio.h>

typedef enum HH_ERRORS
{
  HH_FOPEN_ERROR = -1,
  HH_FREAD_ERROR = -2,
} HH_ERRORS;

/** Throws an improper usage error. */
void improper_usage_error (void);

/**
 *  Throws an fopen error.
 *
 *  @param  filename  name of the file fopen was attempting to open.
 */
void fopen_error (char *filename);

/**
 *  Throws an fread error.
 *
 *  @param  fptr            file pointer that was in use.
 *  @param  actual          actual number of bytes read.
 *  @param  expected_bytes  expected number of bytes to read.
 */

int8_t handle_fread_error (FILE *fptr, size_t actual, size_t expected_bytes);

/**
 *  Throws an unknown command line argument error. This is the only error
 *  throwing function which is allowed to directly call exit(1) as such
 *  an error can only be produced at the start of execution, prior to any
 *  memory allocation.
 *
 *  @param  arg the unrecognized argument passed by the user.
 */
void handle_unknown_command_line_argument_error (char *arg);

#endif
