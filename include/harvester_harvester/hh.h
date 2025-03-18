#ifndef _HARVESTER_HARVESTER_H_
#define _HARVESTER_HARVESTER_H_

/**
 *  param:  input_path   path to some dat file.
 *  param:  output_path  directory where extracted data should be placed.
 *  returns: zero on success, non-zero on failure.
 */
int hh_extract_filesystem (const char input_path[static 1],
                           const char output_path[static 1]);

#endif /* _HARVESTER_HARVESTER_H_ */
