#ifndef _ERRORS_H_
#define _ERRORS_H_

#include <stdio.h>

void improper_usage_error (void);
void fopen_error (char *filename);
void handle_fread_error (FILE *fptr, size_t actual, size_t expected_bytes);

#endif
