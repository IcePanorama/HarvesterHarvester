#ifndef _ERRORS_H_
#define _ERRORS_H_

#include <stdio.h>

void improper_usage_error (void);
void fopen_error (char *filename);
void handle_fread_error (FILE *fptr, size_t actual, size_t expected_bytes);
void handle_unknown_command_line_argument_error (char *arg);

#endif
