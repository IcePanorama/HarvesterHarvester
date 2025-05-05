#ifndef _ISO_9660_FILESYSTEM_H_
#define _ISO_9660_FILESYSTEM_H_

#include <stdio.h>

typedef struct ISO9660FileSystem_s ISO9660FileSystem_t;

ISO9660FileSystem_t *i9660_alloc (void);
void i9660_free (ISO9660FileSystem_t *fs);
void i9660_print (ISO9660FileSystem_t *fs);

/** Processes filesystem header and data sections. */
int i9660_init (ISO9660FileSystem_t *fs, FILE input_fptr[static 1]);
int i9660_extract (ISO9660FileSystem_t *fs, FILE input_fptr[static 1],
                   const char path[static 1]);

#endif /* _ISO_9660_FILESYSTEM_H_ */
