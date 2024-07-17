#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include <stdbool.h>

extern bool debug_mode;
extern char *current_disk_name;
extern char *OPT_INPUT_DIR;
extern bool OPT_BATCH_PROCESS;

void handle_command_line_args (int argc, char **argv);

#endif
