#ifndef _ISO_9660_DIRECTORY_RECORD_H_
#define _ISO_9660_DIRECTORY_RECORD_H_

#include "iso_9660/file_flags.h"

#include <stdint.h>
#include <stdio.h>

typedef struct _DirRec_s _DirRec_t;

_DirRec_t *_dr_alloc (void);
void _dr_free (_DirRec_t *dr);

/** Returns the size of a `_DirRec_t` in bytes. */
size_t _dr_size (void);

/** Returns `(_FileFlags_t)-1` if `dr` is NULL. */
_FileFlags_t _dr_get_flags (_DirRec_t *dr);

/** Returns: Zero on success, non-zero on failure. */
int _dr_init (_DirRec_t *dr, FILE input_fptr[static 1]);
void _dr_print (_DirRec_t *dr);

/**
 *  Initializes a list of directory records all at once, as they're stored back
 *  to back in ISO 9660 file systems.
 *  Returns: Zero on success, non-zero on failure.
 */
int _dr_dynamic_init (_DirRec_t **dr_list, size_t list_cap[static 1],
                      size_t list_len[static 1], uint16_t lb_size,
                      FILE input_fptr[static 1]);

/**
 *  Param:  lb_size logical block size.
 *  Returns: Zero on success, non-zero on failure.
 */
int _dr_extract (_DirRec_t *dr, size_t lb_size, FILE input_fptr[static 1],
                 const char path[static 1], const size_t path_len);

#endif /* _ISO_9660_DIRECTORY_RECORD_H_ */
