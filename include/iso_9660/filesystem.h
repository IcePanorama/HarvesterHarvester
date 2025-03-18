#ifndef _ISO_9660_FILESYSTEM_H_
#define _ISO_9660_FILESYSTEM_H_

#include <stdio.h>

typedef struct ISO9660FileSystem_s ISO9660FileSystem_t;

ISO9660FileSystem_t *i9660_create_fs (void);
int i9660_init_fs (ISO9660FileSystem_t *fs, FILE fptr[static 1]);
void i9660_free_fs (ISO9660FileSystem_t *fs);

#endif /* _ISO_9660_FILESYSTEM_H_ */
