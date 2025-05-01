#ifndef _ISO_9660_PATH_TABLE_ENTRY_H_
#define _ISO_9660_PATH_TABLE_ENTRY_H_

#include <stdint.h>
#include <stdio.h>

typedef struct _PathTableEntry_s _PathTableEntry_t;

extern const size_t _PathTableEntry_SIZE_BYTES;

_PathTableEntry_t *_pte_alloc (void);
void _pte_free (_PathTableEntry_t *p);

uint8_t _pte_get_dir_id_len (const _PathTableEntry_t *e);
/** Returns 1 when `e` is NULL, as directories are 1-indexed. */
uint16_t _pte_get_parent_dir_num (const _PathTableEntry_t *e);
const char *_pte_get_dir_id (const _PathTableEntry_t *e);

int _pte_init (_PathTableEntry_t *p, FILE input_fptr[static 1]);
void _pte_print (_PathTableEntry_t *p);
/** Param:  lb_size logical block size. */
int _pte_extract (_PathTableEntry_t *p, uint16_t lb_size,
                  FILE input_fptr[static 1], const char path[static 1],
                  const size_t path_len);

#endif /* _ISO_9660_PATH_TABLE_ENTRY_H_ */
