#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include <stdbool.h>

extern bool OPT_BATCH_PROCESS;
extern char *OP_CURRENT_DISK_NAME;
extern bool OP_DEBUG_MODE;
extern char *OPT_INPUT_DIR;

void handle_command_line_args (int argc, char **argv);

#endif
