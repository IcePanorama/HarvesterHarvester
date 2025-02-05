#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#include <stddef.h>
#include <stdint.h>

/** Prepends `str` with `prefix`, adding a path separator between them. */
int_fast8_t u_prepend_path_str (char **str, const char prefix[static 1],
                                size_t prefix_len);

#endif /* _UTILITIES_H_ */
