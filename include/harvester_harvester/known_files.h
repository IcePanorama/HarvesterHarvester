#ifndef _HARVESTER_HARVESTER_KNOWN_DAT_FILES_H_
#define _HARVESTER_HARVESTER_KNOWN_DAT_FILES_H_

#include <stdbool.h>

bool _hhkf_is_known_i9660_file (const char path[static 1]);
bool _hhkf_is_known_int_dat (const char path[static 1]);

const char **_hhkf_get_int_dat_paths_from_i9660 (const char path[static 1]);
const char **_hhkf_get_int_idx_paths_from_i9660 (const char path[static 1]);
const char *_hhkf_get_idx_path_from_int_dat (const char path[static 1]);

#endif /* _HARVESTER_HARVESTER_KNOWN_DAT_FILES_H_ */
