#ifndef _ISO_9660_UTILITIES_H_
#define _ISO_9660_UTILITIES_H_

#include <stddef.h>

/**
 *  Assumes `str` is large enough to prepend.
 *  Returns: zero on success, non-zero on failure.
 */
int _u_prepend_str (char str[static 1], const size_t str_len,
                    const char prefix[static 1], const size_t prefix_len);

#endif /* _ISO_9660_UTILITIES_H_ */
