#ifndef _ISO_9660_PRIMARY_VOLUME_DESCRIPTOR_DATE_TIME_H_
#define _ISO_9660_PRIMARY_VOLUME_DESCRIPTOR_DATE_TIME_H_

#include <stdio.h>

typedef struct _PVDDateTime_s _PVDDateTime_t;

_PVDDateTime_t *_pvddt_alloc (void);
void _pvddt_free (_PVDDateTime_t *dt);

int _pvddt_init (_PVDDateTime_t *dt, FILE input_fptr[static 1]);
void _pvddt_print (_PVDDateTime_t *dt, const char dt_id[static 1]);

#endif /* _ISO_9660_PRIMARY_VOLUME_DESCRIPTOR_DATE_TIME_H_ */
