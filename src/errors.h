#ifndef _ERRORS_H_
#define _ERRORS_H_

#include <stdio.h>

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
void handle_fread_error (FILE *fptr, size_t actual, size_t expected_bytes);

/**
 *  Throws an unknown command line argument error.
 *
 *  @param  arg the unrecognized argument passed by the user.
 */
void handle_unknown_command_line_argument_error (char *arg);

#endif
