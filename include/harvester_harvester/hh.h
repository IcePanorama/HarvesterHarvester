#ifndef _HARVESTER_HARVESTER_H_
#define _HARVESTER_HARVESTER_H_

#include "harvester_harvester/options.h"

extern const HHOptions_t HH_DEFAULT_OPTIONS;

/**
 *  param:  input_path   path to some dat file.
 *  param:  output_path  directory where extracted data should be placed.
 *  returns: zero on success, non-zero on failure.
 */
int hh_extract_filesystem (const char input_path[static 1],
                           const char output_path[static 1]);

/**
 *  param:  input_path   path to some dat file.
 *  param:  output_path  directory where extracted data should be placed.
 *  param:  options      customized options for extraction
 *  returns: zero on success, non-zero on failure.
 */
int hh_extract_filesystem_w_options (const char input_path[static 1],
                                     const char output_path[static 1],
                                     const HHOptions_t opts);

#endif /* _HARVESTER_HARVESTER_H_ */
