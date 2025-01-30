#ifndef _HARVESTER_HARVESTER_H_
#define _HARVESTER_HARVESTER_H_

#include <stdint.h>

/** @returns  zero on success, non-zero on failure. */
int8_t hh_extract_filesystem (const char input_file_path[static 1],
                              const char output_dir_path[static 1]);

#endif /* _HARVESTER_HARVESTER_H_ */
