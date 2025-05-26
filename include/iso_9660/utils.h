#ifndef _ISO_9660_UTILITIES_H_
#define _ISO_9660_UTILITIES_H_

#include <stddef.h>
#include <stdint.h>

/**
 *  Assumes `str` is large enough to prepend.
 *  Returns: zero on success, non-zero on failure.
 */
int _u_prepend_str (char str[static 1], const size_t str_len,
                    const char prefix[static 1], const size_t prefix_len);

/**
 *  Create directory given by `path` to be used for exporting data.
 *  Returns: Zero on success, non-zero on failure.
 */
int _u_create_export_dir (const char path[static 1]);

/**
 *  Writes `data` to file at `path`.
 *  Returns: Zero on success, non-zero on failure.
 */
int _u_export_data (uint8_t data[static 1], size_t data_size,
                    const char path[static 1]);

#endif /* _ISO_9660_UTILITIES_H_ */
