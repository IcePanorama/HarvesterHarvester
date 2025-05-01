#ifndef _ISO_9660_FILESYSTEM_HEADER_H_
#define _ISO_9660_FILESYSTEM_HEADER_H_

#include "iso_9660/vol_desc_type_code.h"

#include <stdio.h>

typedef struct _FileSysHeader_s _FileSysHeader_t;

_FileSysHeader_t *_fsh_alloc (void);
void _fsh_free (_FileSysHeader_t *h);

int _fsh_init (_FileSysHeader_t *h, FILE input_fptr[static 1]);
void _fsh_print (_FileSysHeader_t *h);

/**
 *  Returns `VDTC_VOL_DESC_SET_TERMINATOR` when `h` is null.
 *  FIXME: Not really sure how I feel about handling that like this.
 */
_VolDescTypeCode_t _fsh_get_vol_desc_type_code (_FileSysHeader_t *h);

#endif /* _ISO_9660_FILESYSTEM_HEADER_H_ */
