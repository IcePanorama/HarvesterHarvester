#ifndef _EXTRACTOR_H_
#define _EXTRACTOR_H_

#include "directory.h"
#include "path_table.h"

#include <stdint.h>
#include <stdio.h>

int8_t extract_file (FILE *fptr, struct directory_record *dr,
                     const char *path);
int8_t extract_directory (FILE *fptr, const uint16_t BLOCK_SIZE,
                          const char *path);
void create_directories_and_extract_data_from_path_file (
    FILE *fptr, uint16_t BLOCK_SIZE, struct path_table *pt);

#endif
