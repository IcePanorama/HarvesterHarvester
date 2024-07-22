#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include <stdbool.h>

/** If true, batch process all the files in `OP_INPUT_DIR`. */
extern bool OP_BATCH_PROCESS;

// Not really an "option", does this belong here?
extern char *OP_CURRENT_DISK_NAME;

/**
 *  Skips files larger than `DEBUG_FILE_SIZE_LIMIT` if true. Note that
 *  `DEBUG_FILE_SIZE_LIMIT` is currently defined in extractor.c.
 *
 *  @see  extract_directory
 */
// FIXME: `DEBUG_FILE_SIZE_LIMIT` should probably be defined here.
extern bool OP_DEBUG_MODE;

/** Where to look for files if batch processing. Default is `./dat-files/`. */
extern char *OP_INPUT_DIR;

/**
 *  Depends on the user's operating system. Having this be defined globally in
 *  this file for cross-platform compatibility reasons.
 */
extern char OP_PATH_SEPARATOR;

/**
 *  Processes command-line arguments.
 *
 *  @param  argc  number of command-line arguments.
 *  @param  argv  array of all the command-line arguments.
 */
void handle_command_line_args (int argc, char **argv);

#endif
