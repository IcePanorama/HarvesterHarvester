#ifndef _OUTPUT_H_
#define _OUTPUT_H_

#include <stdbool.h>

/** Default is `output/`. */
extern const char *OUTPUT_DIR;

void create_output_directory (char *path);

bool directory_exists (const char *dir);

#endif
