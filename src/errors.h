#ifndef _ERRORS_H_
#define _ERRORS_H_

#include <stdio.h>

/** Throws an improper usage error. */
void improper_usage_error (void);

/** Throws an fopen error. */
void fopen_error (char *filename);

/** Throws an fread error. */
void handle_fread_error (FILE *fptr, size_t actual, size_t expected_bytes);

/** Throws an unknown command line argument error. */
void handle_unknown_command_line_argument_error (char *arg);

#endif
