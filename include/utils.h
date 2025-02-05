#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#include <stddef.h>
#include <stdint.h>

/** Prepends `str` with `prefix`, adding a path separator between them. */
int u_prepend_path_str (char **str, const char prefix[static 1],
                        size_t prefix_len);

/**
 *  Frees a list of elements from [`start_idx`, `end_idx`). Note that this
 *  function does NOT free `list` itself.
 */
void u_free_partial_list_elements (void **list, size_t start_idx,
                                   size_t end_idx);

/**
 *  Frees every element in a given list. Note that this function does NOT free
 *  `list` itself.
 */
void u_free_list_of_elements (void **list, size_t size);

#endif /* _UTILITIES_H_ */
