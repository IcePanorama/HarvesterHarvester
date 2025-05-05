#ifndef _ISO_9660_FILESYSTEM_H_
#define _ISO_9660_FILESYSTEM_H_

#include <stdio.h>

typedef struct ISO9660FileSystem_s ISO9660FileSystem_t;

ISO9660FileSystem_t *i9660_create_fs (void);
void i9660_free_fs (ISO9660FileSystem_t *fs);
/** Processes filesystem header and data sections. */
int i9660_init_fs (ISO9660FileSystem_t *fs, FILE input_fptr[static 1]);
void i9660_print_fs (ISO9660FileSystem_t *fs);
int i9660_extract_fs (ISO9660FileSystem_t *fs, FILE input_fptr[static 1],
                      const char path[static 1]);

#endif /* _ISO_9660_FILESYSTEM_H_ */
