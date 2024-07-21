#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include <stdbool.h>

extern bool OP_BATCH_PROCESS;
extern char *OP_CURRENT_DISK_NAME;
extern bool OP_DEBUG_MODE;
extern char *OP_INPUT_DIR;

void handle_command_line_args (int argc, char **argv);

#endif
