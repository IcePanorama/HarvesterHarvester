#ifndef _ISO_9660_PRIMARY_VOLUME_DESCRIPTOR_H_
#define _ISO_9660_PRIMARY_VOLUME_DESCRIPTOR_H_

#include <stdio.h>

typedef struct _PriVolDesc_s _PriVolDesc_t;

_PriVolDesc_t *_pvd_alloc (void);

/**  FIXME: combine with `_pvd_init`. */
int _pvd_init (_PriVolDesc_t *p, FILE input_fptr[static 1]);
void _pvd_print (_PriVolDesc_t *p);
/**
 *  Populates path table list and directory record list.
 *  FIXME: combine with `_pvd_init`.
 */
int _pvd_process (_PriVolDesc_t *p, FILE input_fptr[static 1]);
void _pvd_free (_PriVolDesc_t *p);
int _pvd_extract (_PriVolDesc_t *p, FILE input_fptr[static 1],
                  const char path[static 1]);

#endif /* _ISO_9660_PRIMARY_VOLUME_DESCRIPTOR_H_ */
